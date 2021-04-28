// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProjectileBase.h"
#include "Components/SphereComponent.h"
#include "PlayerBase.h"

//#define _DEBUG_PROJECTILEBASE

AAIProjectileBase::AAIProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Hitbox"));
	RootComponent = ProjectileHitbox;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned
void AAIProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	Cast<USphereComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &AAIProjectileBase::OnOverlap);
}

void AAIProjectileBase::SetShootDirection(const FVector& NewDirection)
{
	ShootDirection = NewDirection;
}

void AAIProjectileBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#ifdef _DEBUG_PROJECTILEBASE
	UE_LOG(LogTemp, Warning, TEXT("Projectile Overlapped something!"));
#endif
	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
		const auto PlayerController= GetWorld()->GetFirstPlayerController();
		if(PlayerController)
		{
			if (GetOwner()&&GetOwner()->GetInstigatorController()) {
				FDamageEvent DamageEvent;
				PlayerController->GetPawn()->TakeDamage(Damage, DamageEvent,GetOwner()->GetInstigatorController(), this);
#ifdef _DEBUG_PROJECTILEBASE
				UE_LOG(LogTemp, Warning, TEXT("Projectile hit player!"))
#endif
					Destroy();
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT(" Projectile has no owner or owner has no controller! "))
			}
		}
	}
}

// Called every frame
void AAIProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();
	NewLocation += ShootDirection * ProjectileSpeed * DeltaTime;
	SetActorLocation(NewLocation);

	if (TimeSinceCreation > ProjectileLifetime)
		Destroy();
}

