// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIProjectileBase.generated.h"

UCLASS()
class HOOKGAME_API AAIProjectileBase : public AActor
{
	GENERATED_BODY()

		FVector ShootDirection{};	
public:	
	AAIProjectileBase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basics")
		UStaticMeshComponent* ProjectileMesh{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basics")
		class USphereComponent* ProjectileHitbox{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
		float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
		float ProjectileSpeed = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
		float ProjectileLifetime = 5.f;
	
	void SetShootDirection(const FVector& NewDirection);
	
	
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float TimeSinceCreation = 0.f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
