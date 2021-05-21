#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "CableComponent.h"
#include "Engine/DataTable.h"
#include "../PlayerControllers/PlayerControllerBase.h"
#include "Actors/HookProjectile.h"
#include "../WorldActors/CameraRotateTrigger.h"
#include "Actors/PlayerSword.h"
#include "../Ai/AIRobotSidekick.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkillTree.h"
#include "Widgets/MainHUD.h"
#include "../Components/Dialogue.h"
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
namespace AttackStateTransition
{
	enum AttackStateTransition
	{
		Default, Attack1, Attack2
	};
}
namespace HookCurrentState
{
	enum HookCurrentState
	{
		GrappleDetached,
		GrappleInAir,
		GrappleTo,
		GrappleHit,
		GrappleReturn,
		GrappleFired
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

	friend class UHookGameInstance;
	friend class USkillTree;
	friend class USkillTreeObject;
	
public:
	APlayerBase();

	UFUNCTION(BlueprintCallable)
	USkillTree* GetSkillTree() const;
private:

	bool bIsRunning{ false };
	bool bGodMode{ false };
	
	UPROPERTY(EditAnywhere, Category="Skill tree")
	USkillTree* SkillTreeFCRI;

	void DebugUpdate(const float DeltaTime) const;

	void MovementUpdate(const float DeltaTime);

	void ApplyUpgrades();
	
	void CreateComponents();
	
	void GrappleInit();

	void SkillTreeInit(_Inout_ APlayerController* PlayerController);
	
	void WidgetInit(_Inout_ APlayerController* PlayerController);

	void SetMouseCursor(_Inout_ APlayerController* PlayerController, const EMouseCursor::Type NewType);
	
	void InteractInit(_Inout_ APlayerControllerBase* PlayerController);
	
	void CameraInit();
	
	void CharacterInit();
	
	void VaultInit();
	
	void SwordInit();
	
	void ShadowProjectionInit();
	
	void InteractLoad();
	
	void CharacterLoad();
	
	void HandleDeath();
	
	FTimerHandle MyTimerHandle;
	
	void RestartLevel();
	
	//**Character bindings //Events **//
	//-----------------------------------------------------------
	void OnMoveRight(const float Value);
	
	void OnMoveForward(const float Value);
	
	void OnUsePressed();
	
	void OnJumpPressed();
	
	void OnJumpReleased();
	
	void OnRightClickPressed();
	
	void OnRightClickReleased();

	void CursorUpdate(const float DeltaTime);
	
	//Hook Functions
	//** Updates hook every frame **//
	void HookUpdate(const float DeltaTime);
	
	//** Deletes hook projectile from the world**//
	void HookDelete();
	
	//** Detaches from currently attached object**//
	void HookDetach();
	
	//** Calculates player velocity when grappling towards an static object**//
	void HookFire();
	
	void TryEnterHookFireTransition();

	void GrappleTo(const float DeltaTime);
	
	void GrappleReturn(const float DeltaTime);
	
	void CalculatePlayerVelocityWhileRetracting(const float DeltaTime);

	bool CompareHookState(HookCurrentState::HookCurrentState NewState) const;
	
	void SetHookState(HookCurrentState::HookCurrentState NewState);
	
	HookCurrentState::HookCurrentState GetHookState() const;
	
	void SetCustomMovement(CustomMovementModes::CustomMovementModes NewState);
	
	CustomMovementModes::CustomMovementModes GetCustomMovement() const;
	
	bool CompareCustomMovement(CustomMovementModes::CustomMovementModes NewState) const;
	
	void SetAttackStateTransition(AttackStateTransition::AttackStateTransition NewState);
	
	void SetAttackState(AttackState::AttackState NewState);
	
	bool CompareAttackState(AttackState::AttackState NewState) const;
	
	bool CompareAttackStateTransition(AttackStateTransition::AttackStateTransition NewState) const;
	
	void GrappleDetachPhysicsComponent();
	
	//** Not Unit**//
	FVector GetDirectionToHook() const;
	
	FRotator GetRotationToHook() const;
	
	float GetDistanceToHook() const;
	
	//Vaulting Functions
	void VaultingUpdate(const float DeltaTime);
	
	void TryEnterVaultingTransition(const FVector& EndPoint);
	
	void StartVaulting();
	
	void EndVaulting();

	UPROPERTY(EditAnywhere, Category = "Vault Box Extent")
	UBoxComponent* VaultBoxExtent {	nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Vault Capsule Extent")
	UCapsuleComponent* VaultCapsule {nullptr};
	
	//Sword
	UPROPERTY(EditAnywhere, Category="Sword Settings")
	TSubclassOf<APlayerSword> SwordClass;
	
	UPROPERTY()
	APlayerSword* SwordPtr{ nullptr };

	float InitialPlayerSpeed{ 0.f };//Taking this from character movement

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SwordAttackWalkSpeed{ 60.f };
	
	void SwordStartAttack();
	
	void SwordEndAttack();

	//Camera update
	void CameraUpdate(const float DeltaTime);
	
	//**Update Camera Based on triggers placed in the world*//
	void CameraRotationUpdate(const float DeltaTime);

	void CameraPositionUpdate(const float DeltaTime) const;
	
	//** Max Camera Extent will limit this**//
	void CameraMoveTo2DDirection(const FVector2D& Dir, const float DeltaTime) const;
	
	void CameraMoveToPlayer(const float DeltaTime) const;
	
	FRotator CameraInitialRotation{ FRotator::ZeroRotator };
	
	FVector CameraInitialPosition{ FVector::ZeroVector };

	FVector2D GetPlayerToMouseDirScreenSpace2D() const;
	
	FVector GetPlayerToMouseDirScreenSpace3D() const;
	
	FRotator GetPlayerToMouseYawScreenSpace() const;

	UPROPERTY(EditAnywhere, Category="Aim Assist Settings")
	FVector AimAssistBoundingBox{ 20.f, 200.f, 1000.f };
	
	UPROPERTY(EditAnywhere, Category = "Aim Assist Settings")
	bool bShowAimAssistBoundingBox{ false };
	
	FVector CalculateGrappleFireDirection() const;
public:
	//** Events triggered when player enters/leaves a camera trigger box **//
	void OnBeginCameraTrigger(ACameraRotateTrigger* Trigger);
	
	void OnEndCameraTrigger(ACameraRotateTrigger* Trigger) const;

	UFUNCTION()
	void OnSkillTreePressed();
private:
	//** Current Associated trigger box **//
	UPROPERTY()
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

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	float PlayerOnKnockBackValue{ 2000.f };

	float CapsuleDefaultStandingHeight{ 84.f };
	
	float CapsuleDefaultCrouchedHeight{ 64.f };
	
	float CapsuleDefaultRadius{ 24.f };
	
	//states
	ECollisionChannel AttachedComponentType;
	
	HookCurrentState::HookCurrentState _HookCurrentState;
	
	CustomMovementModes::CustomMovementModes _CustomMovementMode = CustomMovementModes::Default;
	
	AttackStateTransition::AttackStateTransition _AttackStateTransition = AttackStateTransition::Default;
	
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
	
	UPROPERTY()
	AHookProjectile* Hook{nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Hook Settings")
	UCableComponent* GrappleCable{nullptr};
	
	UPROPERTY()
	UPrimitiveComponent* AttachedComponent{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UMainHUD> MainHUDClass;

	UPROPERTY()
	UMainHUD* PlayerHUD;
	
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> DeadScreenClass;
	
	UUserWidget* DeadScreen{nullptr};//Allow garbage collection

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> WidgetClassCursorNoneLock;
	UPROPERTY()
	UUserWidget* WidgetCursorNotLocked{nullptr};

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> WidgetClassCursorLocked;
	UPROPERTY()
	UUserWidget* WidgetCursorLocked {nullptr};

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> WidgetClassMenu;
	UPROPERTY()
	UUserWidget* WidgetMenu {nullptr};

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> WidgetClassSword;
	UPROPERTY()
	UUserWidget* WidgetSword {nullptr};

	UPROPERTY()
	class USceneCaptureComponent2D* ShadowProjectionCamera { nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Shadow Projection Texture")
	class UTextureRenderTarget2D* ShadowProjectionTexture{nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Shadow Projection Camera")
	USpringArmComponent* ShadowProjectionCameraArm{nullptr};
	
	UPROPERTY()
	class USceneCaptureComponent2D* DepthProjectionCamera{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Depth Projection Texture")
	class UTextureRenderTarget2D* DepthProjectionTexture{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RunSpeed{ 800.f };

	void OnShiftPressed();
	void OnShiftReleased();

	//Sounds
	UPROPERTY(EditAnywhere, Category=Sounds)
	USoundBase* HookFireSound{nullptr};
	
	UPROPERTY(EditAnywhere, Category=Sounds)
	USoundBase* HookHitSound{nullptr};
	
	//-----------------------------------------------------------
protected:
	virtual void OnFirePressed();
	
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
	void OnAttack1End();
	
	UFUNCTION(BlueprintCallable)
	void OnAttack2End();
	
	UFUNCTION(BlueprintCallable)
	bool IsAttack1() const;
	
	UFUNCTION(BlueprintCallable)
	bool CanTransitionToAttack1() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsAttack2() const;
	
	UFUNCTION(BlueprintCallable)
	bool CanTransitionToAttack2() const;

	UFUNCTION(BlueprintCallable)
	void OnHookFire();
	
	//Movement Functions
	UFUNCTION(BlueprintPure)
	bool IsSliding() const;

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable)
	bool IsGrappleFired() const;

	UFUNCTION(BlueprintCallable)
	bool IsGrappleReturning() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsGrapplingTo() const;

	UFUNCTION(BlueprintCallable)
	bool IsGrappling() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsHookInAir();
	
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

	virtual void OnUpdatePlayerCustomTexture2DUniform(class FViewUniformShaderParameters& ViewUniformShaderParameters) override;
	virtual void OnUpdatePlayerCustomCbUniform(TStaticArray<FVector4, 8>& DataPins) override;

	//Load Checkpoint system	
	UFUNCTION()
	void LoadPlayerData();
	
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	
	UFUNCTION(BlueprintCallable)
	void LoadGame();
	
	UFUNCTION(BlueprintCallable)
	void SaveLatestCheckpoint();

	UFUNCTION(BlueprintCallable)
	void LoadCheckpoint();

	UFUNCTION(BlueprintPure)
	float GetPlayerToHookRotation() const;

	//PowerUp/HealthUp
	UFUNCTION()
	float GetCurrentHealth() const;

	UFUNCTION()
	float GetMaxHealth() const;
	
	UFUNCTION()
	void InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InteractionNotActive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnIncomingXP(int32 Amount) const;
	
	void GeneratePlayerLines(UDataTable* PlayerLines2);

	void OnPlayerKnockBack(const FVector& KnockBackFrom, float KnockBackValue) const;

	void OnHookHitGrappleTo();
	void OnHookHitGrappleInAttachedComponent(_Inout_ UPrimitiveComponent* OtherComp);

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Questions;

	void SetTalkRangeStatus(bool Status) { PlayerCanInteract = Status; }

	void SetAssociatedPawn(AAIRobotSidekick* Pawn) { AssociatedNPC = Pawn; }
	
	UFUNCTION(BlueprintCallable, Category = "Dialogue System")
	void SetInteractableNPC(AAIRobotSidekick* NPC);
	
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget> InteractionPopup;
	UPROPERTY()
	UUserWidget* InteractionWidget;

	UPROPERTY()
	class AAIRobotSidekick* InteractableNPC;

	FVector NPCPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueSystem")
	float InteractSizeX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueSystem")
	float InteractSizeY;

	UFUNCTION(BlueprintCallable, Category = "Dialogue System")
	void DisplayInteractMessage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueSystem")
	float SpeakerNPCDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNPCFollowPlayer = false;

	//Vaulting
	UFUNCTION()
	void OnVaultBoxBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
	bool bAimAssistEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInMenu{ false };

	//ToolTip

	UFUNCTION()
	void ToolTipTrigger(float Duration, FString Text);

	UFUNCTION()
	void ToolTipTextUpdate(float DeltaTime);
	
	//CheckpointWidget
	UFUNCTION()
	void CheckPointWidget(float DeltaTime);

	UPROPERTY()
	bool bDisplayCheckWidget = false;

	bool bSkillTreeOpened = false;

	void LevelUpWidgetUpdate();

	void OnPlayerLevelUp();

	void PlayerHUDInit();

	void ShowDialogueUI();

	void HideDialogueUI();

	UPROPERTY(EditAnywhere, Category = "DialogueSystem")
	UDataTable* PlayerLines;

	UPROPERTY(EditAnywhere, Category = "DialogueSystem")
	UDataTable* NPCLines;

	void TalkWithNPC(FName index);

	bool bTalkingWithNPC = false;

	FName CurrentIndex = "";

	bool bPlayerIsTalking;

	bool bPlayerIsCurrentlyTalking = false;

	float PlayerTalkTimer{ 0.f };

	float RobotTalkTimer{ 0.f };
	
	int AllLinesCurrentIndex = 0;

	void UpdateTalking(float DeltaTime);

protected:
	//Called to interact
	void InteractWithNPC();

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComp;

	void ToggleUI();
	
	UPROPERTY(BlueprintReadOnly)
	bool PlayerCanInteract;

private:
	
	UPROPERTY()
	AAIRobotSidekick* AssociatedNPC;
	
	UPROPERTY()
	UDataTable* AvailableLines;

	FDialogue* RetrieveDialogue(UDataTable* TableToSearch, FName RowName);
		
	//Tooltip
	TArray<TPair<float, FString>> ToolTipTextArray;

	float ToolTipTimer = 0.f;

	//Checkpoint
	UPROPERTY(EditAnywhere, Category = "Widgets")
	float CheckpointWidgetDisplayTime = 10.f;

	float CheckPointTimer = 0.f;
		
};









