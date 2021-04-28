// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIBehaviorBase.generated.h"

UCLASS()
class HOOKGAME_API AAIBehaviorBase : public ACharacter
{
	GENERATED_BODY()

	
		friend class UExperienceComponent;
public:
	// Sets default values for this character's properties
	AAIBehaviorBase();

	void OnAttach();
	void OnDetach();

	UPROPERTY(BlueprintReadWrite)
		bool bIsGrabbed = false;
	UPROPERTY(BlueprintReadWrite)
		bool bIsAttacking = false;
	UPROPERTY(BlueprintReadWrite)
		bool bIsDead = false;
	UPROPERTY(EditAnywhere, Category = "Movement")
		bool bPatrollingEnabled = false;					// If set to true, the actor will patrol with the following variables. If set to false, it will not have a patrolling system.
	UPROPERTY(EditAnywhere, Category = "Movement")
		FVector PatrolDistance {0.f, 0.f, 0.f};	// Distance the actor will move from their start location when patrolling
	UPROPERTY(EditAnywhere, Category = "Movement")
		float PatrolEndTurnAngle = 180.f; // How much the actor will turn after they have moved their patrol distance. Default is to turn around completely and return to start location.
	FRotator PatrolEndTurnAngleRotation{ 0.f,0.f,PatrolEndTurnAngle };
	UPROPERTY(BlueprintReadWrite)
		bool bKnockedBack=false;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float PatrolEndTurnSpeed = 0.5f; // The speed the actor turns upon finishing their patrol distance, value kept between 0-1.
	UPROPERTY(EditAnywhere, Category = "Behavior")
		float DetectionRange = 100.f; // Max range the actor will react to the player

	UPROPERTY(EditAnywhere, Category = "Behavior")
		float ResetTimer = 4.f; // Time before actor resets, and returns to start position

	UPROPERTY(EditAnywhere)
		float TimeUntilDespawn = 10.f;
	
	virtual void OnBeginOverlapAttack();
	virtual void OnEndOverlapAttack();
	virtual void Attack();
	virtual void EndAttack();
	virtual void HandleDeath();
	virtual void FollowPlayer();
	virtual void DamageKnockback(AActor* DamageCauser);
	float WeightModifier();

	UPROPERTY(EditAnywhere, Category="Grapple settings")
	float StunDurationAfterGrabbed{ 1.f };
	float CurrentStunDuration{ 0.f };
	bool bCanAttack{ true };
	void UpdateStunDuration(const float DeltaTime);
	
	bool bPlayerInRange = false;
	bool bReturnToStart = true;
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

	UPROPERTY(EditAnywhere, Category = "Stats")
		float RecoveryTime = 3.f; // How long after the player throws/knocks back the enemy does it get back up?

	float TimeSinceAttack = 0.f;

	float TimeSinceDeath = 0.f;

	float TimeSinceReleased = 10.f;

	FRotator StandingRotation { 0.f,0.f,0.f };
	FVector StandingPosition{ 0.f,0.f,0.f };
	float StandingZValue=0.f;
	bool bIsStanding = true;
	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
