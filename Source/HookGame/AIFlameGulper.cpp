// Fill out your copyright notice in the Description page of Project Settings.


#include "AIFlameGulper.h"
#include "PlayerBase.h"
#include "Components/SphereComponent.h"
#include "AIProjectileBase.h"

//#define _DEBUG_FLAMEGULPER
AAIFlameGulper::AAIFlameGulper()
{
	ProjectileOffset = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Offset"));
	ProjectileOffset->SetupAttachment(RootComponent);
}

void AAIFlameGulper::BeginPlay()
{
	Super::BeginPlay();
	
}


void AAIFlameGulper::Attack()
{
#ifdef _DEBUG_FLAMEGULPER
	UE_LOG(LogTemp,Warning,TEXT("Flamegulper is attacking!"))
#endif
	bIsAttacking = true;
}

void AAIFlameGulper::ShootProjectile()
{
	if(GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		UWorld* world = GetWorld();
		if(ProjectileBP)
		{
			auto const Projectile = world->SpawnActor<AAIProjectileBase>(ProjectileBP, ProjectileOffset->GetComponentLocation(), GetActorRotation(), SpawnParams);
			ReturnIfNull(Projectile);
			const auto Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			const auto PlayerLocation = Player->GetActorLocation();
			const auto AiLocation = GetActorLocation();
			const auto AiToPlayer = PlayerLocation - AiLocation;
			const auto AiToPlayerDir = AiToPlayer / AiToPlayer.Size();
			Projectile->SetShootDirection(AiToPlayerDir);
		}
	}
}

void AAIFlameGulper::EndAttack()
{
	bIsAttacking = false;
}


void AAIFlameGulper::HandleDeath()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsAttacking = false;
	bIsDead = true;
}


