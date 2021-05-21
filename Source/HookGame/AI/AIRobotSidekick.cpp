// Fill out your copyright notice in the Description page of Project Settings.


#include "AIRobotSidekick.h"
#include "../GameInstance/HookGameInstance.h"
#include "../Character/PlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HookGame/Helpers/Macros.h"

AAIRobotSidekick::AAIRobotSidekick()
{
	PropellerBlade = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Propeller Blade"));
	PropellerBlade->SetupAttachment(RootComponent);

	InteractionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	InteractionComp->SetupAttachment(RootComponent);

	InitialRotation = this->GetActorRotation();
}

void AAIRobotSidekick::BeginPlay()
{
	Super::BeginPlay();
	SetStartValues();
}

void AAIRobotSidekick::SetStartValues()
{
	
}

void AAIRobotSidekick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckDistanceToPlayer();
}

float AAIRobotSidekick::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	UE_LOG(LogTemp, Error, TEXT(" Robots feel no pain! "));
	return 0;
}

void AAIRobotSidekick::SetFollowStatus(bool FollowStatus)
{

	if(FollowStatus == true)
	{
		bInteractable = false;
		bFollowPlayer = true;
	}
	else
	{
		bInteractable = true;
		bFollowPlayer = false;
	}
	
}

void AAIRobotSidekick::CheckDistanceToPlayer()
{
	const auto Player = GetWorld()->GetFirstPlayerController();
	ReturnIfNull(Player);
	ReturnIfNull(GetCharacterMovement());
	
	float DistanceFromPlayer = FVector::Dist(Player->GetPawn()->GetActorLocation(), GetActorLocation());
	if(DistanceFromPlayer<MaximumDistanceToPlayer)
	{
		bRunningToCatchUp = false;
		GetCharacterMovement()->MaxFlySpeed = RobotMovespeed;
		return;
	}
	
	if(DistanceFromPlayer>MaximumDistanceToPlayer)
	{
		bRunningToCatchUp = true;
		GetCharacterMovement()->MaxFlySpeed = RobotMovespeed * CatchupSpeedMultiplier;
		return;
	}	
}


