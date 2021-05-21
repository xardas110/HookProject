// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueenSquidbirdController.h"
#include "AIQueenSquidBase.h"
#include "../Character/PlayerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "HookGame/Helpers/Macros.h"

void AAIQueenSquidbirdController::Tick(float DeltaSeconds)
{
	SetBlackboardValues();
}

void AAIQueenSquidbirdController::SetBlackboardValues()
{
	ReturnIfNull(GetWorld());
	ReturnIfNull(GetBlackboardComponent());
	const auto PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	ReturnIfNull(PlayerPawn);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsInspecting"), GetPawn<AAIQueenSquidBase>()->bIsInspecting);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), GetPawn<AAIQueenSquidBase>()->bIsDead);
}
