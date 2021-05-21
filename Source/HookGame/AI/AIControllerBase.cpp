// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"
#include "AIBehaviorBase.h"
#include "../Character/PlayerBase.h"
#include "AIRobotSidekick.h"
#include "HookGame/Helpers/Macros.h"

//#define _DEBUG_AICONTROLLER

void AAIControllerBase::SetBlackboardStartValues()
{
	ReturnIfNull(AIBehavior);
	RunBehaviorTree(AIBehavior);
	ReturnIfNull(GetBlackboardComponent());
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ReturnIfNull(PlayerPawn);

	if (!GetPawn())
		return;
	
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), GetPawn<AAIBehaviorBase>()->bPlayerInRange);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("ResetTimer"), GetPawn<AAIBehaviorBase>()->ResetTimer);

	if (GetPawn<AAIRobotSidekick>())
		SetRobotSidekickBlackboardStartValues();
}

void AAIControllerBase::SetRobotSidekickBlackboardStartValues()
{
	ReturnIfNull(GetPawn<AAIRobotSidekick>());
	GetBlackboardComponent()->SetValueAsBool(TEXT("FollowPlayer"), GetPawn<AAIRobotSidekick>()->bFollowPlayer);
#ifdef _DEBUG_AICONTROLLER
	UE_LOG(LogTemp, Warning, TEXT(" Set FollowPlayer=True!"));
#endif
}

void AAIControllerBase::SetRobotSidekickBlackboardChangingValues()
{
	ReturnIfNull(GetBlackboardComponent());
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ReturnIfNull(PlayerPawn);
	
	GetBlackboardComponent()->SetValueAsBool(TEXT("FollowPlayer"), GetPawn<AAIRobotSidekick>()->bFollowPlayer);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

	if (LineOfSightTo(PlayerPawn))
	{
		bCanSeePlayer = true;
	}
	else
	{
		bCanSeePlayer = false;
	}
	GetBlackboardComponent()->SetValueAsBool(TEXT("CanSeePlayer"), bCanSeePlayer);
}

void AAIControllerBase::SetBlackboardChangingValues()
{
	ReturnIfNull(GetBlackboardComponent());
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ReturnIfNull(PlayerPawn);

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), GetPawn<AAIBehaviorBase>()->bIsDead);
	GetBlackboardComponent()->SetValueAsBool(TEXT("bCanAttack"), GetPawn<AAIBehaviorBase>()->bCanAttack);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsKnockedBack"), GetPawn<AAIBehaviorBase>()->bKnockedBack);
	GetBlackboardComponent()->SetValueAsVector(TEXT("DistanceToPlayer"), PlayerPawn->GetActorLocation() - GetPawn()->GetActorLocation());
	
	GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), GetPawn<AAIBehaviorBase>()->bPlayerInRange);
	GetBlackboardComponent()->SetValueAsBool(TEXT("ReturnToStart"), GetPawn<AAIBehaviorBase>()->bReturnToStart);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsGrabbed"), GetPawn<AAIBehaviorBase>()->bIsGrabbed);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttacking"), GetPawn<AAIBehaviorBase>()->bIsAttacking);
	
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerPawn))
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerDead"), PlayerBase->IsDead());
	
	if (LineOfSightTo(PlayerPawn)) { // Checks if the AI Can see the player, and therefore if it can chase them.
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

		const auto ThisAI = GetPawn<AAIBehaviorBase>();
		if (auto LinkedAi = ThisAI->LinkedAi)
			LinkedAi->OnPlayerSpotted(ThisAI, PlayerPawn->GetActorLocation());
	}
	else {
		
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
#ifdef _DEBUG_AICONTROLLER
			UE_LOG(LogTemp, Warning, TEXT(" %s Can't see player "), *GetPawn()->GetName());
#endif
	}

}


AAIControllerBase::AAIControllerBase() {
	
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SetBlackboardStartValues();
	
}

void AAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetBlackboardChangingValues();
	if (GetPawn<AAIRobotSidekick>())
		SetRobotSidekickBlackboardChangingValues();
}
