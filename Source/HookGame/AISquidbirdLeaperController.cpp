// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquidbirdLeaperController.h"
#include "AISquidbirdLeaper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

//#define _DEBUG_LEAPERCONTROLLER

void AAISquidbirdLeaperController::BeginPlay()
{
	Super::BeginPlay();
	if (GetPawn<AAISquidbirdLeaper>())
	{
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsFloat(TEXT("LeapDistance"), GetPawn<AAISquidbirdLeaper>()->LeapDistance);
#ifdef _DEBUG_LEAPERCONTROLLER
		UE_LOG(LogTemp, Warning, TEXT("Found AIBehavior!"));
#endif
		GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolLocation"), GetPawn()->GetActorLocation() + GetPawn<AAISquidbirdLeaper>()->PatrolDistance);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), GetPawn<AAISquidbirdLeaper>()->bPlayerInRange);
		GetBlackboardComponent()->SetValueAsFloat(TEXT("ResetTimer"), GetPawn<AAISquidbirdLeaper>()->ResetTimer);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsPatrolling"), GetPawn<AAISquidbirdLeaper>()->bPatrollingEnabled);
		
	}
}

void AAISquidbirdLeaperController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		if (GetPawn<AAISquidbirdLeaper>())
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("LeapTarget"), GetPawn<AAISquidbirdLeaper>()->LeapTarget);

			if (PlayerPawn)
			{
				GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), GetPawn<AAIBehaviorBase>()->bIsDead);
				if (LineOfSightTo(PlayerPawn)) { // Checks if the AI Can see the player, and therefore if it can chase them.
					GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
					GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
				}
				else
				{
#ifdef _DEBUG_LEAPERCONTROLLER
					UE_LOG(LogTemp, Error, TEXT("Leaper can't see player!"));
#endif
				}
			}
		}
}

