// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProjectileBase.h"
#include "Components/SphereComponent.h"
#include "../Character/PlayerBase.h"
#include "AIFlameGulper.h"
#include "HookGame/Helpers/Macros.h"

//#define _DEBUG_PROJECTILEBASE

AAIProjectileBase::AAIProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Bullet Hitbox"));
	RootComponent = ProjectileHitbox;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileHitbox->OnComponentHit.AddDynamic(this, &AAIProjectileBase::OnOverlap);

	EnemyProjectile=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Component"));
	EnemyProjectile->SetUpdatedComponent(ProjectileHitbox);
	EnemyProjectile->InitialSpeed = BaseProjectileSpeed;
	EnemyProjectile->MaxSpeed = BaseProjectileSpeed;
	EnemyProjectile->bRotationFollowsVelocity = true;
	EnemyProjectile->bShouldBounce = true;
	EnemyProjectile->Bounciness = 0.3f;
	EnemyProjectile->ProjectileGravityScale = 0.0f;
}

void AAIProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SetStartValues();
	SetLifeSpan(ProjectileLifetime);
}

void AAIProjectileBase::SetShootDirection(const FVector& NewDirection)
{
	EnemyProjectile->Velocity = NewDirection * EnemyProjectile->InitialSpeed;
}

void AAIProjectileBase::SetStartValues()
{
	ReturnIfNull(GetOwner());
	BaseProjectileSpeed = Cast<AAIFlameGulper>(GetOwner())->ProjectileSpeed;
	Damage = Cast<AAIFlameGulper>(GetOwner())->ProjectileDamage;
}

void AAIProjectileBase::OnOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
#ifdef _DEBUG_PROJECTILEBASE
	UE_LOG(LogTemp, Warning, TEXT("Projectile Overlapped something!"));
#endif
	if (OtherActor->IsA(APlayerBase::StaticClass()))
	{
		const auto PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			if (GetOwner() && GetOwner()->GetInstigatorController()) {
				FDamageEvent DamageEvent;
				PlayerController->GetPawn()->TakeDamage(Damage, DamageEvent, GetOwner()->GetInstigatorController(), this);
#ifdef _DEBUG_PROJECTILEBASE
				UE_LOG(LogTemp, Warning, TEXT("Projectile hit player!"))
#endif
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT(" Projectile has no owner or owner has no controller! "))
			}
		}
	}
	Destroy();
}

void AAIProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

