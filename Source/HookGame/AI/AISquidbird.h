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
	///			VARIABLES		///

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
	
	///			Functions		///

	UFUNCTION(BlueprintCallable)
		virtual void OnBeginOverlapAttack() override;

	UFUNCTION(BlueprintCallable)
		virtual void OnEndOverlapAttack() override;

	UFUNCTION(BlueprintCallable)
		virtual void EndAttack() override;
	
	virtual void Attack() override;
	virtual void Dodge() override;
	virtual void HandleDeath() override;
	void SetStartValues();
protected:
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	bool bInRange = false;
};
