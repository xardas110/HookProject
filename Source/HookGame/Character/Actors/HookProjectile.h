#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "HookProjectile.generated.h"

UCLASS()
class HOOKGAME_API AHookProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	USceneComponent* CableAttachmentPoint;
	UPROPERTY(EditAnywhere, Category = "Settings")
	USphereComponent* HookCollision;
	UPROPERTY(EditAnywhere, Category = "Settings")
	class USkeletalMeshComponent* HookMesh;
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere, Category = "Settings")
	float Damage{ 50.f };
	
public:	
	// Sets default values for this actor's properties
	AHookProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UProjectileMovementComponent* GetProjectileMovementComponent();
	USphereComponent* GetCollisionComponent();
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ShootDirection(const FVector Direction);
	USceneComponent* GetCableAttachmentPoint() const;
	void SetSpeed(const float Speed);
	void SetMeshRotation(const FRotator NewRotation);
};
