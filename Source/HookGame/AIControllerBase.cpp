// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"
#include "AIBehaviorBase.h"
#include "AIRobotSidekick.h"

//#define _DEBUG_AICONTROLLER

void AAIControllerBase::PatrolFinished() // On a patrolling enemy, sets new rotation and destination
{
	if (GetPawn<AAIBehaviorBase>()) {

		FRotator Rotation = FMath::Lerp(GetOwner()->GetActorRotation(), GetPawn<AAIBehaviorBase>()->PatrolEndTurnAngleRotation, GetPawn<AAIBehaviorBase>()->PatrolEndTurnSpeed);
		GetOwner()->SetActorRotation(Rotation);

		if (Rotation == GetPawn<AAIBehaviorBase>()->PatrolEndTurnAngleRotation) {
			bPatrolFinished = false;
			GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), GetPawn()->GetActorLocation() + GetPawn<AAIBehaviorBase>()->PatrolDistance);
		}
		
	}
}

AAIControllerBase::AAIControllerBase() {
	
}

void AAIControllerBase::BeginPlay() {
	Super::BeginPlay();

	
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (AIBehavior != nullptr) { // if AIBehavior exists, Sets all blackboard key values
		RunBehaviorTree(AIBehavior);
		if (!GetBlackboardComponent())
		{
			return;
		}
		
		
		
		if (GetPawn<AAIBehaviorBase>())
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
			GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), GetPawn()->GetActorLocation() + GetPawn<AAIBehaviorBase>()->PatrolDistance);
			GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), GetPawn<AAIBehaviorBase>()->bPlayerInRange);
			GetBlackboardComponent()->SetValueAsFloat(TEXT("ResetTimer"), GetPawn<AAIBehaviorBase>()->ResetTimer);
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsPatrolling"), GetPawn<AAIBehaviorBase>()->bPatrollingEnabled);
		}
		if(GetPawn<AAIRobotSidekick>())
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("FollowPlayer"), GetPawn<AAIRobotSidekick>()->bFollowPlayer);
#ifdef _DEBUG_AICONTROLLER
			UE_LOG(LogTemp, Warning, TEXT(" Set FollowPlayer=True!"));
#endif
		}
		else
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("FollowPlayer"), false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" *AIBehavior not found!* "));
	}
}

void AAIControllerBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!GetBlackboardComponent())
	{
		return;
	}
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), GetPawn<AAIBehaviorBase>()->bIsDead);
		GetBlackboardComponent()->SetValueAsBool(TEXT("bCanAttack"), GetPawn<AAIBehaviorBase>()->bCanAttack);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsKnockedBack"), GetPawn<AAIBehaviorBase>()->bKnockedBack);
		GetBlackboardComponent()->SetValueAsVector(TEXT("DistanceToPlayer"), PlayerPawn->GetActorLocation() - GetPawn()->GetActorLocation());
		if (LineOfSightTo(PlayerPawn)&&!GetPawn<AAIRobotSidekick>()) { // Checks if the AI Can see the player, and therefore if it can chase them.
			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
		}
		else {
			if(!GetPawn<AAIRobotSidekick>())
			{
			GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
#ifdef _DEBUG_AICONTROLLER
			UE_LOG(LogTemp, Warning, TEXT(" %s Can't see player "), *GetPawn()->GetName());
#endif
			}
		}

		if (GetPawn<AAIBehaviorBase>() && !GetPawn<AAIRobotSidekick>()) {
			GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), GetPawn<AAIBehaviorBase>()->bPlayerInRange);
			GetBlackboardComponent()->SetValueAsBool(TEXT("ReturnToStart"), GetPawn<AAIBehaviorBase>()->bReturnToStart);
		}

		if (bPatrolFinished == true) {
			PatrolFinished();
		}

		if (GetPawn<AAIBehaviorBase>() && !GetPawn<AAIRobotSidekick>()) {
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsGrabbed"), GetPawn<AAIBehaviorBase>()->bIsGrabbed);
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttacking"), GetPawn<AAIBehaviorBase>()->bIsAttacking);
		}

		if(GetPawn<AAIRobotSidekick>())
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("FollowPlayer"), GetPawn<AAIRobotSidekick>()->bFollowPlayer);
			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
			
			if(LineOfSightTo(PlayerPawn))
			{
				bCanSeePlayer = true;
			}
			else
			{
				bCanSeePlayer = false;
			}
			GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeePlayer"), bCanSeePlayer);
		}
	}
}
