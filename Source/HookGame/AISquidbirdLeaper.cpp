// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquidbirdLeaper.h"


#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

void AAISquidbirdLeaper::Attack()
{
		bIsAttacking = true;
		BeginAttack();
}

void AAISquidbirdLeaper::BeginAttack()
{
	UWorld* GameWorld = GetWorld();
	if(GameWorld)
	{
		const auto PlayerController = GameWorld->GetFirstPlayerController();
		if(PlayerController)
		{
			LeapTarget=PlayerController->GetPawn()->GetActorLocation();
			FVector Direction = LeapTarget - GetActorLocation();
				Direction /= Direction.Size();
			GetCharacterMovement()->Velocity += Direction*LeapForce;
		}
		
	}
}

void AAISquidbirdLeaper::Leap()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	
	
}

void AAISquidbirdLeaper::EndAttack()
{
	bIsAttacking = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AAISquidbirdLeaper::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(bIsAttacking)
	{
		Leap();
	}
}
