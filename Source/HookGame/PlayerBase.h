// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CableComponent.h"
#include "HookProjectile.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "CameraRotateTrigger.h"
#include "PlayerSword.h"
#include "Blueprint/UserWidget.h"
#include "PlayerBase.generated.h"

namespace CustomMovementModes
{
	enum CustomMovementModes
	{
		Default,
		Sliding = 1,
		Vaulting,
		Attacking,
		Grappling
	};
}
namespace AttackState
{
	enum AttackState
	{
		Default, Attack1, Attack2
	};
}
namespace HookCurrentState
{
	enum HookCurrentState
	{
		GrappleDetached,
		GrappleTo,
		GrappleReturn,
		GrappleFired,
		GrappleFiredWithObject
	};
}

inline float QuadraticFormula(const float x, const float a, const float b, const float c)
{
	return (a * powf(x, 2)) + (b * x) + c;
}

UCLASS()
class HOOKGAME_API APlayerBase : public ACharacterBase
{
	GENERATED_BODY()
public:
	APlayerBase();
private:

	void DebugUpdate(const float DeltaTime) const;
	void CreateComponents();
	
	void GrappleInit();
	void WidgetInit();
	void CharacterInit();
	void SwordInit();

	void HandleDeath();
	
	//**Character bindings //Events **//
	//-----------------------------------------------------------
	void OnMoveRight(const float Value);
	void OnMoveForward(const float Value);
	void OnUsePressed();
	void OnUseReleased();
	void OnJumpPressed();
	void OnJumpReleased();
	void OnRightClickPressed();
	void OnRightClickReleased();
	
	//Hook Functions
	//** Updates hook every frame **//
	void HookUpdate(const float DeltaTime);
	//** Deletes hook projectile from the world**//
	void HookDelete();
	//** Detaches from currently attached object**//
	void HookDetach();
	//** Calculates player velocity when grappling towards an static object**//
	void HookFire();
	bool HookCanFire();
	
	void GrappleDetachPhysicsComponent();
	//** Not Unit**//
	FVector GetDirectionToHook() const;
	float GetDistanceToHook() const;
	
	const FVector CalculatePlayerVelocityWhileRetracting(const float DeltaTime);

	//Vaulting Functions
	void VaultingUpdate(const float DeltaTime);
	void TryEnterVaultingTransition(const FVector& EndPoint);
	void StartVaulting();
	void EndVaulting();

	TSubclassOf<AActor> SpawnThisClass;
	AActor* SpawnThis = nullptr;

	//Sword
	UPROPERTY(EditAnywhere, Category="Sword Settings")
	TSubclassOf<APlayerSword> SwordClass;
	APlayerSword* SwordPtr{ nullptr };
	void SwordStartAttack();
	void SwordEndAttack();

	//Camera update
	void CameraUpdate(const float DeltaTime);
	//**Update Camera Based on triggers placed in the world*//
	void CameraRotationUpdate(const float DeltaTime);
	void CameraPositionUpdate(const float DeltaTime);
	//** Max Camera Extent will limit this**//
	void CameraMoveTo2DDirection(const FVector2D& Dir, const float DeltaTime);
	void CameraMoveToPlayer(const float DeltaTime);
	FRotator CameraInitialRotation{ FRotator::ZeroRotator };
	FVector CameraInitialPosition{ FVector::ZeroVector };

public:
	//** Events triggered when player enters/leaves a camera trigger box **//
	void OnBeginCameraTrigger(ACameraRotateTrigger* Trigger);
	void OnEndCameraTrigger(ACameraRotateTrigger* Trigger);
private:
	//** Current Associated trigger box **//
	ACameraRotateTrigger* AssociatedTriggerBox{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	FVector2D MaxCameraExtent {600.f, 600.f};
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraLerpToSpeed{1000.f};
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraLerpBackSpeed{ 1.f };
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float EdgeScale{ 0.03f };

	//-----------------------------------------------------------
	//Hook Variables
	//-----------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookMaxRange{ 500.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookFireSpeed{ 2000.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookConstantAddSpeed{ 100.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookInRange{ 100.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookRetractMaxSpeed{ 1200.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookReturnSpeed{ 1200.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookRetractStartSpeed{ 400.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookOnAttachedLaunchForce{ 400.f };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookDetachMaxTimer{ 1000.f };
	float HookDetachTimer{ HookDetachTimer };
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	float HookMaxDelay{ 1.f };
	float HookDelayTimer{ 0.f };
	float InitialDistanceToHook{ 0.f };
	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	float DefaultGroundFriction{ 5.f };
	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	float DefaultAirControl{ 1.f };

	float CapsuleDefaultStandingHeight{ 84.f };
	float CapsuleDefaultCrouchedHeight{ 64.f };
	float CapsuleDefaultRadius{ 24.f };

	//Vectors
	FVector HookSpawnLocation{};
	
	//states
	ECollisionChannel AttachedComponentType;
	HookCurrentState::HookCurrentState _HookCurrentState;
	CustomMovementModes::CustomMovementModes _CustomMovementMode = CustomMovementModes::Default;
	public: AttackState::AttackState _AttackState = AttackState::Default;
	private:
	//-----------------------------------------------------------
	
	//**Components**//
	UPROPERTY(EditAnywhere, Category = "Camera Properties")
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, Category = "Camera Properties")
	USpringArmComponent* SpringArm;
	//**Hook Components**//	
	UPROPERTY(EditAnywhere, Category = "Hook settings")
	TSubclassOf<AHookProjectile> HookProjectileClass;
	AHookProjectile* Hook{nullptr};
	UPROPERTY(EditAnywhere, Category = "Hook Settings")
	UCableComponent* GrappleCable;
	UPrimitiveComponent* AttachedComponent{ nullptr };
	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<UUserWidget> WidgetClassCursorNoneLock;
	UUserWidget* WidgetCursorNotLocked{nullptr};
	//-----------------------------------------------------------
protected:
	virtual void OnFirePressed();
	virtual void OnFireReleased();

	UPROPERTY(EditAnywhere, Category = "Health Properties")
	float MaxHealth{ 100.f };
	float CurrentHealth{ 0.f };

	virtual void BeginPlay() override;
public:

	UFUNCTION(BlueprintCallable)
	void OnVaultingBegin_TransitionStart();
	UFUNCTION(BlueprintCallable)
	void OnVaultingBegin_TransitionInterrupted();
	UFUNCTION(BlueprintCallable)
	void OnVaultingBegin_TransitionEnd();
	UFUNCTION(BlueprintCallable)
	void OnVaultingEnd_TransitionStart();
	UFUNCTION(BlueprintCallable)
	void OnVaultingEnd_TransitionInterrupted();
	UFUNCTION(BlueprintCallable)
	void OnVaultingEnd_TransitionEnd();

	void TryEnterSwordAttackTransition();
	UFUNCTION(BlueprintCallable)
	void OnSwordAttackBegin_TransitionStart();
	UFUNCTION(BlueprintCallable)
	void OnSwordAttackBegin_TransitionInterrupted();
	UFUNCTION(BlueprintCallable)
	void OnSwordAttackBegin_TransitionEnd();
	UFUNCTION(BlueprintCallable)
	void OnSwordAttackEnd_TransitionStart();
	UFUNCTION(BlueprintCallable)
	void OnSwordAttackEnd_TransitionInterrupted();
	UFUNCTION(BlueprintCallable)
	void OnSwordAttackEnd_TransitionEnd();

	int32 AttackPressedWhileSwinging{ 0 };
	int32 AttackCurrentComboNR{ 0 };
	void HandleAttackCombo();
	UFUNCTION(BlueprintCallable)
	void OnAttack1StartWindow();
	UFUNCTION(BlueprintCallable)
	void OnAttack1EndWindow();
	UFUNCTION(BlueprintCallable)
	bool IsAttack1() const;
	bool bCanTransitionToAttack1{ false };
	bool bAttack1WindowOpen{ false };
	UFUNCTION(BlueprintCallable)
	bool CanTransitionToAttack1() const;
	UFUNCTION(BlueprintCallable)
	void OnAttack2StartWindow();
	UFUNCTION(BlueprintCallable)
	void OnAttack2EndWindow();
	UFUNCTION(BlueprintCallable)
	bool IsAttack2() const;
	bool bCanTransitionToAttack2{ false };
	bool bAttack2WindowOpen{ false };
	UFUNCTION(BlueprintCallable)
	bool CanTransitionToAttack2() const;

	//Movement Functions
	UFUNCTION(BlueprintPure)
	bool IsSliding() const;

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const;
	
	UFUNCTION(BlueprintPure)
	bool CanVault() const;
	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure)
	float GetHookDelayPercent() const;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

