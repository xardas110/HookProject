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
	const auto GameInstance = Cast<UHookGameInstance>(GetGameInstance());
	ReturnIfNull(GameInstance);
	
	if (GameInstance->bNPCDataLoad)
	{
		GameInstance->LoadNPCData(this);
		GameInstance->bNPCDataLoad = false;
		}
	if (GameInstance->bNPCCheckpointDataLoad)
	{
		GameInstance->LoadNPCCheckpoint(this);
		GameInstance->bNPCCheckpointDataLoad = false;
	}
	if (bInteractable)
	{
		bFollowPlayer = false;
		InteractionComp->OnComponentBeginOverlap.AddDynamic(this, &AAIRobotSidekick::InteractionActive);
		InteractionComp->OnComponentEndOverlap.AddDynamic(this, &AAIRobotSidekick::InteractionNotActive);
	}
	if (!bInteractable)
	{
		bFollowPlayer = true;

	}
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


void AAIRobotSidekick::InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerBase>())
	{
		if(bInteractable)
		{
			/*UE_LOG(LogTemp, Warning, TEXT("NPC registered that you are in the interaction area"));*/
			APlayerBase* Char = Cast<APlayerBase>(OtherActor);
			Char->SetTalkRangeStatus(true);
			Char->SetAssociatedPawn(this);
			Char->SetInteractableNPC(this);
			Char->GeneratePlayerLines(Char->PlayerLines);
			Char->DisplayInteractMessage();
		}

	}

}

void AAIRobotSidekick::InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	
	if (OtherActor->IsA<APlayerBase>())
	{
		if(bInteractable)
		{
			/*UE_LOG(LogTemp, Warning, TEXT("NPC registered that you are no longer in the interaction area"));*/
			APlayerBase* Char = Cast<APlayerBase>(OtherActor);
			Char->SetTalkRangeStatus(false);
			Char->SetAssociatedPawn(nullptr);
			Char->SetInteractableNPC(nullptr);
			SetFollowStatus(Char->bNPCFollowPlayer);
			this->SetActorRotation(InitialRotation);

			bool PlayerBaseFollowStatus = Char->bNPCFollowPlayer;

			if(Char->bNPCFollowPlayer)
			{
				this->SetActorRotation(InitialRotation);
			}

			//UE_LOG(LogTemp, Warning, TEXT("PlayerBaseFollowStatus is set to: %s"), (PlayerBaseFollowStatus ? TEXT("true") : TEXT("false")));
			//UE_LOG(LogTemp, Warning, TEXT("NPCFollowStatus is set to: %s"), (bFollowPlayer ? TEXT("true") : TEXT("false")));
			//UE_LOG(LogTemp, Warning, TEXT("NPCinteractStatus is set to: %s"), (bInteractable ? TEXT("true") : TEXT("false")));
			
		}
		
	}
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


