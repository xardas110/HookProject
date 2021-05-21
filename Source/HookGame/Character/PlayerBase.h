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

UCLASS()
class HOOKGAME_API APlayerBase : public ACharacterBase
{
	GENERATED_BODY()

	
public:
	APlayerBase();

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
	
	void GeneratePlayerLines(UDataTable* PlayerLines2);


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









