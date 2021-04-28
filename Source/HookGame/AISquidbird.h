// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviorBase.h"
#include "AISquidbird.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAISquidbird : public AAIBehaviorBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	AAISquidbird();

	UFUNCTION(BlueprintCallable)
		virtual void OnBeginOverlapAttack() override;

	UFUNCTION(BlueprintCallable)
		virtual void OnEndOverlapAttack() override;

	virtual void Attack() override;

	UFUNCTION(BlueprintCallable)
		virtual void EndAttack() override;
	
	virtual void Dodge() override;

	virtual void HandleDeath() override;
	
	UPROPERTY(BlueprintReadWrite)
		bool bIsDodging = false;
	
	UPROPERTY(EditAnywhere, Category = "Squidbird Params");
	class USphereComponent* AttackRange{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Squidbird Params");
	class USphereComponent* AttackHitbox{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Squidbird Params");
	bool bHasRabies = false;

	UPROPERTY(EditAnywhere, Category = "Squidbird Params");
	float AttackDamage = 10.f;

protected:
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bInRange = false;

};
