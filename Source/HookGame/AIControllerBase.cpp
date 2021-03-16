// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIParamaterComponent.h"
#include "Math/UnrealMathUtility.h"
#include "AIBehaviorBase.h"

void AAIControllerBase::PatrolFinished()
{
	if (OwnerParameters) {

		FRotator Rotation = FMath::Lerp(GetOwner()->GetActorRotation(), OwnerParameters->PatrolEndTurnAngleRotation, OwnerParameters->PatrolEndTurnSpeed);
		GetOwner()->SetActorRotation(Rotation);

		if (Rotation == OwnerParameters->PatrolEndTurnAngleRotation) {
			bPatrolFinished = false;
			GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), GetPawn()->GetActorLocation() + OwnerParameters->PatrolDistance);
		}
		
	}
}

AAIControllerBase::AAIControllerBase() {
}

void AAIControllerBase::BeginPlay() {
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (AIBehavior != nullptr) {
		RunBehaviorTree(AIBehavior);
		//UAIParamaterComponent* OwnerParameters = Cast<UAIParamaterComponent>(GetPawn()->GetOwner()->FindComponentByClass<UActorComponent>());
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		if (GetPawn()->FindComponentByClass<UAIParamaterComponent>() == nullptr) {
			UE_LOG(LogTemp, Error, TEXT(" AN AI ACTOR IS MISSING THEIR AI PARAMATERS COMPONENT!"));
		}
		else {
		//UE_LOG(LogTemp, Warning, TEXT("%s Exists"), *GetPawn()->FindComponentByClass<UAIParamaterComponent>()->GetName());
		OwnerParameters = GetPawn()->FindComponentByClass<UAIParamaterComponent>();
		}
		if (OwnerParameters != nullptr) {
			GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), GetPawn()->GetActorLocation() + OwnerParameters->PatrolDistance);
			GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), OwnerParameters->bPlayerInRange);
			GetBlackboardComponent()->SetValueAsFloat(TEXT("ResetTimer"), OwnerParameters->ResetTimer);
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsPatrolling"), OwnerParameters->bPatrollingEnabled);
			
		}
		
	}
}

void AAIControllerBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	

	if (LineOfSightTo(PlayerPawn)) {
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
	}
	else {
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	}

	if (OwnerParameters != nullptr) {
		GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), OwnerParameters->bPlayerInRange);
		GetBlackboardComponent()->SetValueAsBool(TEXT("ReturnToStart"), OwnerParameters->bReturnToStart);
	}

	if (bPatrolFinished == true) {
		PatrolFinished();
	}

	if (GetPawn<AAIBehaviorBase>()) {
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsGrabbed"), GetPawn<AAIBehaviorBase>()->bIsGrabbed);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttacking"), GetPawn<AAIBehaviorBase>()->bIsAttacking);
	}
}
