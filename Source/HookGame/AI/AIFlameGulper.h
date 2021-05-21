// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviorBase.h"
#include "AIFlameGulper.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIFlameGulper : public AAIBehaviorBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		class USphereComponent* ProjectileOffset{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		TSubclassOf<class AAIProjectileBase> ProjectileBP{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float ProjectileSpeed{ 1000.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float ProjectileDamage{ 10.f };
	
	UFUNCTION(BlueprintCallable)
		void ShootProjectile();

	UFUNCTION(BlueprintCallable)
		virtual void EndAttack() override;

	virtual void Attack() override;
	virtual void HandleDeath() override;

	protected:
	AAIFlameGulper();
	virtual void BeginPlay() override;
	FVector AttackTarget{ 0.f,0.f,0.f };
	
};
