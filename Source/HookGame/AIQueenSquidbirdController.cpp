// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueenSquidbirdController.h"
#include "AIQueenSquidBase.h"
#include "PlayerBase.h"

#include "BehaviorTree/BlackboardComponent.h"

void AAIQueenSquidbirdController::Tick(float DeltaSeconds)
{
	if (GetWorld()) {
		const auto PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (GetBlackboardComponent()&&PlayerPawn)
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsInspecting"), GetPawn<AAIQueenSquidBase>()->bIsInspecting);
			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
			UE_LOG(LogTemp, Warning, TEXT("Warning!"));
		}
	}
}
