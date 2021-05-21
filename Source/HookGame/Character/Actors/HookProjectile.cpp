// Fill out your copyright notice in the Description page of Project Settings.


#include "HookProjectile.h"
#include "../PlayerBase.h"
#include "DrawDebugHelpers.h"

AHookProjectile::AHookProjectile()
{

	PrimaryActorTick.bCanEverTick = true;

	HookCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = HookCollision;
	HookCollision->OnComponentHit.AddDynamic(this, &AHookProjectile::OnHit);
	
	HookMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hook Mesh"));
	HookMesh->SetupAttachment(HookCollision);

	CableAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Cable Attachment Point"));
	CableAttachmentPoint->SetupAttachment(HookCollision);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(HookCollision);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
}


void AHookProjectile::BeginPlay()
{
	Super::BeginPlay();
}

UProjectileMovementComponent* AHookProjectile::GetProjectileMovementComponent()
{
	return ProjectileMovementComponent;
}

USphereComponent* AHookProjectile::GetCollisionComponent()
{
	return HookCollision;
}

void AHookProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
		return;
	
	if (APlayerBase* Player = Cast<APlayerBase>(GetOwner()))
		Player->OnHookHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);	
}

// Called every frame
void AHookProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHookProjectile::ShootDirection(const FVector Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

USceneComponent* AHookProjectile::GetCableAttachmentPoint() const
{
	return CableAttachmentPoint;
}

void AHookProjectile::SetSpeed(const float Speed)
{
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
}

void AHookProjectile::SetMeshRotation(const FRotator NewRotation)
{
	HookMesh->SetRelativeRotation(NewRotation);
}

