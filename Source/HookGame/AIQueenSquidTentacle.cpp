// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueenSquidTentacle.h"

#include "Components/CapsuleComponent.h"
#include "PlayerBase.h"



AAIQueenSquidTentacle::AAIQueenSquidTentacle()
{
	BossPlatform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	BossPlatform->SetupAttachment(RootComponent);
	AttackHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Attack Hitbox"));
	AttackHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("AttackSocket")));
	
}

void AAIQueenSquidTentacle::BeginPlay()
{
	Super::BeginPlay();
	AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AAIQueenSquidTentacle::OnBeginOverlap);
	AttackHitbox->SetGenerateOverlapEvents(false);
	PlatformTarget = GetActorLocation();
	PlatformDownedTarget = PlatformTarget;
	PlatformDownedTarget.Z = PlatformDownedTarget.Z - PlatformFallDistance;
	
	if (bAlwaysAttacking)
		Attack();
}

void AAIQueenSquidTentacle::TentacleBeginAttackDown()
{
#ifdef _DEBUG_SQUIDTENTACLE
	UE_LOG(LogTemp, Warning, TEXT("Begin attack down called"));
#endif
	AttackHitbox->SetGenerateOverlapEvents(true);
}

void AAIQueenSquidTentacle::TentacleDrownPlatform(float DeltaSeconds)
{
#ifdef _DEBUG_SQUIDTENTACLE
	if (GetOwner()) {
		UE_LOG(LogTemp, Warning, TEXT("%s Platform Hit by tentacle called"), *GetOwner()->GetName());
	}
#endif
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewLocation.Z - (PlatformFallSpeed * DeltaSeconds);
	SetActorLocation(NewLocation);
}


void AAIQueenSquidTentacle::EndAttack()
{
#ifdef _DEBUG_SQUIDTENTACLE
	UE_LOG(LogTemp, Warning, TEXT("Tentacle attack ended"));
#endif
	bIsAttacking = false;
	bPlatformIsWalkable = true;
	AttackHitbox->SetGenerateOverlapEvents(false);
}

void AAIQueenSquidTentacle::Attack()
{
#ifdef _DEBUG_SQUIDTENTACLE
	if (GetOwner()) {
		UE_LOG(LogTemp, Warning, TEXT("%s attack called"), *GetOwner()->GetName());
	}
#endif
	bIsAttacking = true;
}

void AAIQueenSquidTentacle::RespawnPlatform(float DeltaSeconds)
{
#ifdef _DEBUG_SQUIDTENTACLE
	if (GetOwner()) {
		UE_LOG(LogTemp, Warning, TEXT("%s Respawn Platform called"), *GetOwner()->GetName());
	}
#endif
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewLocation.Z + (PlatformFallSpeed * DeltaSeconds);
	SetActorLocation(NewLocation);
}

void AAIQueenSquidTentacle::PlatformHitByTentacle()
{
	bPlatformIsWalkable = false;
}

void AAIQueenSquidTentacle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#ifdef _DEBUG_SQUIDTENTACLE
	UE_LOG(LogTemp, Warning, TEXT("Overlapping something"));

#endif
	if (OtherActor->IsA(APlayerBase::StaticClass())) {
#ifdef _DEBUG_SQUIDTENTACLE
		UE_LOG(LogTemp, Warning, TEXT("Overlapping Player"));
#endif
		const auto PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			FDamageEvent DamageEvent;
			PlayerController->GetPawn()->TakeDamage(TentacleDamage, DamageEvent, Controller, this);
#ifdef _DEBUG_SQUIDTENTACLE
			UE_LOG(LogTemp, Warning, TEXT("Dealt Damage to player"));
#endif
		}
		else
		{
#ifdef _DEBUG_SQUIDTENTACLE
			UE_LOG(LogTemp, Error, TEXT("PlayerController not found!"));
#endif
		}
	}
}

void AAIQueenSquidTentacle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TimeSinceAttack += DeltaSeconds;
	if (!bIsDead) {
		if (bAlwaysAttacking)
		{
			if (TimeSinceAttack > TimeUntilAttack) {
				Attack();
				TimeSinceAttack = 0.f;
			}
		}
		if (bPlatformIsWalkable)
		{
			if (GetActorLocation().Z < PlatformTarget.Z)
			{
				RespawnPlatform(DeltaSeconds);
			}
		}
		if (!bPlatformIsWalkable)
		{
			if (GetActorLocation().Z > PlatformDownedTarget.Z)
			{
				TentacleDrownPlatform(DeltaSeconds);
			}
		}
	}
}
