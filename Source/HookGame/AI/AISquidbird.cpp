// Fill out your copyright notice in the Description page of Project Settings.


#include "AISquidbird.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/PlayerBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HookGame/Helpers/Macros.h"

//#define _DEBUG_SQUIDBIRD

AAISquidbird::AAISquidbird()
{
	AttackHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Hitbox"));
	AttackHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("AttackSocket")));
}

void AAISquidbird::BeginPlay()
{
	Super::BeginPlay();
	SetStartValues();
}

void AAISquidbird::OnBeginOverlapAttack() {

	AttackHitbox->SetGenerateOverlapEvents(true);
#ifdef _DEBUG_SQUIDBIRD
	UE_LOG(LogTemp, Warning, TEXT("SetGenerateOverlapEvents = true"));
#endif
}

void AAISquidbird::OnEndOverlapAttack()
{
	AttackHitbox->SetGenerateOverlapEvents(false);
#ifdef _DEBUG_SQUIDBIRD
	UE_LOG(LogTemp, Warning, TEXT("SetGenerateOverlapEvents = false"));
#endif
}

void AAISquidbird::Attack()
{
	bIsAttacking = true;
	if (!bHasRabies) {
		GetCharacterMovement()->DisableMovement();
	}
#ifdef _DEBUG_SQUIDBIRD
	UE_LOG(LogTemp, Warning, TEXT("Set bIsAttacking = true"));
#endif
}

void AAISquidbird::EndAttack()
{
	AAIBehaviorBase::EndAttack();
	bIsAttacking = false;
	if (!bHasRabies) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
#ifdef _DEBUG_SQUIDBIRD
	UE_LOG(LogTemp, Warning, TEXT("Set bIsAttacking = false"));
#endif
}

void AAISquidbird::Dodge()
{
	if (bPlayerInRange)
	{
		AddActorLocalOffset(DodgeOffset);
	}
}

void AAISquidbird::HandleDeath()
{
#ifdef _DEBUG_SQUIDBIRD
	UE_LOG(LogTemp, Error, TEXT("Handle death"));
#endif
	bIsAttacking = false;
	bIsDodging = false;
	bIsDead = true;
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAISquidbird::SetStartValues()
{
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AAISquidbird::OnBeginOverlap);
	AttackHitbox->SetGenerateOverlapEvents(false);
}

void AAISquidbird::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
	const auto PlayerController = GetWorld()->GetFirstPlayerController();
	ReturnIfNull(PlayerController);

		if (!DamagedActors.Contains(OtherActor))
		{
			FDamageEvent DamageEvent;
			PlayerController->GetPawn()->TakeDamage(AttackDamage, DamageEvent, Controller, this);
			DamagedActors.Push(OtherActor);
		}
#ifdef _DEBUG_SQUIDBIRD
		UE_LOG(LogTemp, Warning, TEXT("Dealt Damage to player"));
#endif
	}
}


