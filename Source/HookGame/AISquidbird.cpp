// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquidbird.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBase.h"
#include "AIParamaterComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AAISquidbird::AAISquidbird()
{
	AttackHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Hitbox"));
	AttackHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("AttackSocket")));
}

void AAISquidbird::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AAISquidbird::OnBeginOverlap);
}

void AAISquidbird::OnBeginOverlapAttack() {

	AttackHitbox->SetGenerateOverlapEvents(true);
	UE_LOG(LogTemp, Warning, TEXT("SetGenerateOverlapEvents = true"));
}

void AAISquidbird::OnEndOverlapAttack()
{
	AttackHitbox->SetGenerateOverlapEvents(false);
	UE_LOG(LogTemp, Warning, TEXT("SetGenerateOverlapEvents = false"));
}

void AAISquidbird::Attack()
{
	bIsAttacking = true;
	GetCharacterMovement()->DisableMovement();
	UE_LOG(LogTemp, Warning, TEXT("Set bIsAttacking = true"));
}

void AAISquidbird::EndAttack()
{
	bIsAttacking = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	UE_LOG(LogTemp, Warning, TEXT("Set bIsAttacking = false"));
}

void AAISquidbird::Dodge()
{
	UAIParamaterComponent* OwnerParams =FindComponentByClass<UAIParamaterComponent>();
	if (OwnerParams)
	{
		if (OwnerParams->bPlayerInRange)
		{
			AddActorLocalOffset(DodgeOffset);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player not in range!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't get Paramater Component!"));
	}
}

void AAISquidbird::HandleDeath()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void AAISquidbird::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
		const auto PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			FDamageEvent DamageEvent;
			PlayerController->GetPawn()->TakeDamage(10.f, DamageEvent, Controller, this);
			UE_LOG(LogTemp, Warning, TEXT("Dealt Damage to player"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController not found!"));
		}
	}
}


