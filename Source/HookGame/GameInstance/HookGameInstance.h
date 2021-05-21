#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HookGameInstance.generated.h"

UCLASS()
class HOOKGAME_API UHookGameInstance : public UGameInstance
{
	GENERATED_BODY()
		
public:
	UHookGameInstance();
	
	UFUNCTION()
	void SaveGameData(AActor* Player);

	UFUNCTION()
	void SaveCommonPlayerData(class USavingAndLoading* SaveGameInstance, class APlayerBase* PlayerRef);

	UFUNCTION()
	void LoadCommonPlayerData(class USavingAndLoading* LoadGameInstance, class APlayerBase* PlayerRef);
	
	UFUNCTION()
	void LoadGameData(AActor* Player);

	UFUNCTION()
	void LoadPlayerData(class APlayerBase* PlayerBase);

	UFUNCTION()
	void LoadNPCData(class AAIRobotSidekick* Robot);

	UFUNCTION()
	void SaveLatestCheckpoint(AActor* Player);

	UFUNCTION()
	void LoadLatestCheckpoint(AActor* Player);

	UFUNCTION()
	void LoadPlayerCheckpointData(class APlayerBase* PlayerBase);

	UFUNCTION()
	void LoadNPCCheckpoint(AAIRobotSidekick* Robot);

	UPROPERTY()
	bool bLevelIsLoaded = false;

	UPROPERTY()
	bool bCheckpointIsLoaded = false;

	UPROPERTY()
	bool bNPCDataLoad = false;

	UPROPERTY()
	bool bNPCCheckpointDataLoad = false;
};


