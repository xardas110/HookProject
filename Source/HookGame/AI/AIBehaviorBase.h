// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Components/GrappleTracer.h"
#include "AIBehaviorBase.generated.h"

UCLASS()
class HOOKGAME_API AAIBehaviorBase : public ACharacter
{
	GENERATED_BODY()

	friend class UExperienceComponent;
	friend class UGrappleTracer;
	friend class AAIControllerBase;
	
	UPROPERTY(EditAnywhere)
	AAIBehaviorBase* LinkedAi;//If this AI sees the player it will tell other AI's around it
	bool bFollowLinkedAiCommand{ false };
	
	void UpdateAi(_In_ APawn* Player, const float DeltaTime);
	void UpdateDodge(_Inout_ class APlayerBase* Player, const float DeltaTime);
	void CheckIfPlayerIsInRange(const _In_ FVector& MyLocation, const _In_ FVector& PlayerLocation, _Inout_ float& PlayerLastSeen,const float DeltaTime);
public:

	UPROPERTY(EditAnywhere, Category = Tracer)
	UGrappleTracer* GrappleTracer;
	
	// Sets default values for this character's properties
	AAIBehaviorBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	
	UPROPERTY(BlueprintReadWrite)
		bool bIsGrabbed = false;
	UPROPERTY(BlueprintReadWrite)
		bool bIsAttacking = false;
	UPROPERTY(BlueprintReadWrite)
		bool bIsDead = false;
	UPROPERTY(BlueprintReadWrite)
		bool bKnockedBack=false;
	UPROPERTY(EditAnywhere)
		float KnockbackImmunityTime = 1.f;

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
	void DeathDespawnTimer(float DeltaSeconds);
	float WeightModifier();
	void StunRecoveryTimer(float DeltaTime);
	void OnAttach();
	void OnDetach();
	virtual void OnPlayerSpotted(const _In_ AAIBehaviorBase* Parent, const _In_ FVector& PlayerLocation);
	void UpdateStunDuration(const float DeltaTime);
	
	UPROPERTY(EditAnywhere, Category="Grapple settings")
	float StunDurationAfterGrabbed{ 1.f };
	float CurrentStunDuration{ 0.f };
	bool bCanAttack{ true };
	bool bPlayerInRange = false;
	bool bReturnToStart = true;
	
protected:
	UPROPERTY()
	TArray<AActor*> DamagedActors;
	
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

	FRotator StandingRotation { 0.f,0.f,0.f };
	FVector StandingPosition{ 0.f,0.f,0.f };
	float TimeSinceAttack = 0.f;
	float TimeSinceDeath = 0.f;
	float TimeSinceReleased = 10.f;
	float TimeSinceKnockedBack = 0.f;
	float StandingZValue=0.f;
	bool bIsStanding = true;
	bool bKnockbackImmune = false;
};
