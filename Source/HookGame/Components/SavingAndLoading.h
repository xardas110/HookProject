#pragma once

#include "CoreMinimal.h"
#include "../Character/Components/SkillTree.h"
#include "GameFramework/SaveGame.h"
#include "SavingAndLoading.generated.h"

UCLASS()
class HOOKGAME_API USavingAndLoading : public USaveGame
{
	GENERATED_BODY()

public:
	USavingAndLoading();

	UPROPERTY(EditAnywhere)
	FVector PlayerLocation;
	
	UPROPERTY(EditAnywhere)
	FVector PlayerRotation;

	UPROPERTY(EditAnywhere)
	float SavedHealth;

	UPROPERTY(EditAnywhere)
	FString SavedLevel;

	UPROPERTY(EditAnywhere)
	bool NPCFollowStatus;

	UPROPERTY(EditAnywhere)
	FRotator CameraSpringarmRotation;

	UPROPERTY()
	uint32 SkillTreeLevel {0};
	UPROPERTY()
	uint32 CurrentExperience {0};
	UPROPERTY()
	uint32 SkillPoints {0};
	UPROPERTY()
	int32 SwordKnockRank {0};
	UPROPERTY()
	int32 SwordLengthRank {0};
	UPROPERTY()
	int32 GrappleSpeedRank {0};
	

	void SaveLevelName(FName Level);

	void OpenLevel();
};
