// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIBehaviorBase.generated.h"

UCLASS()
class HOOKGAME_API AAIBehaviorBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIBehaviorBase();

	void OnAttach();
	void OnDetach();

	UPROPERTY(BlueprintReadWrite)
		bool bIsGrabbed = false;

	UPROPERTY(BlueprintReadWrite)
		bool bIsAttacking = false;

	bool bIsDead = false;

	virtual void OnBeginOverlapAttack();
	virtual void OnEndOverlapAttack();
	virtual void Attack();
	virtual void EndAttack();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Dodge();

	UPROPERTY(EditAnywhere, Category = "Stats")
		float HealthTotal = 10.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
		int DodgeChance = 100;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
		FVector DodgeOffset{0.f,0.f,0.f};

	UPROPERTY(EditAnywhere, Category = "Stats")
		float DodgeCooldown = 1.f;

	float TimeSinceAttack = 0.f;

	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
