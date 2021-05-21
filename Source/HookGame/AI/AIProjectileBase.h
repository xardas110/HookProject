// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AIProjectileBase.generated.h"

UCLASS()
class HOOKGAME_API AAIProjectileBase : public AActor
{
	GENERATED_BODY()

		FVector ShootDirection{};	
public:
	AAIProjectileBase();
	
	UPROPERTY(EditAnywhere,Category="Basics")
		UProjectileMovementComponent* EnemyProjectile{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basics")
		UStaticMeshComponent* ProjectileMesh{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basics")
		class USphereComponent* ProjectileHitbox{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
		float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
		float BaseProjectileSpeed = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
		float ProjectileLifetime = 5.f;
	
	void SetShootDirection(const FVector& NewDirection);
	void SetStartValues();
	void UpdateLocation(float DeltaTime);
	virtual void Tick(float DeltaTime) override;
		
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;
	float TimeSinceCreation = 0.f;
};
