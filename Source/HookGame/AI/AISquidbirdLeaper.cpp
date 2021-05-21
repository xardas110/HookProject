#include "AISquidbirdLeaper.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HookGame/Helpers/Macros.h"

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

void AAISquidbirdLeaper::SetGrabbedVelocity()
{
	if (!bIsGrabbed)
	{
		bVelocityReduced = false;
		return;
	}
	else if(bVelocityReduced)
	{
		
	}
	else if(!bVelocityReduced)
	{
		GetCharacterMovement()->Velocity = { 0.f,0.f,0.f };
		bVelocityReduced = true;
		bIsAttacking=false;
		return;
	}
}

void AAISquidbirdLeaper::EndAttack()
{
	AAIBehaviorBase::EndAttack();
	bIsAttacking = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AAISquidbirdLeaper::KeepGrounded()
{
	float CurrentZLocation = GetActorLocation().Z;
	if(CurrentZLocation>StandingZValue+MaxZIncrease)
	{
		FVector NewLocation{ GetActorLocation().X,GetActorLocation().Y,StandingZValue + MaxZIncrease };
		SetActorLocation(NewLocation);
	}
}

void AAISquidbirdLeaper::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bIsAttacking)
	{
		Leap();
	}
	SetGrabbedVelocity();
	KeepGrounded();
}
