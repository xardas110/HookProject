// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API UAbilityBase : public UDataAsset
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UAbilityBase();

	UPROPERTY(EditAnywhere, Category = "Settings")
	float UpgradeRatio{ 0.33f };
	
	UPROPERTY(EditAnywhere,  Category = "Settings")
		int32 CurrentRank {
		0
	};
	UPROPERTY(EditAnywhere,  Category = "Settings")
		int32 MaxRank {
		3
	};
	UPROPERTY(EditAnywhere, Category = "Settings")
		FString ToolTip {
		"Tool Tip Text"
	};
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FString Title;

	void InitButton(class UButton* ButtonRef, class USkillTree* SkillTreeRef, float* ValuePtr);

	UPROPERTY(EditAnywhere)
	class UButton* Button;

	float Base{ 0.f };

	float* PtrToVal;

	UPROPERTY()
	class USkillTree* SkillTreePtr;

	bool CanRankUp() const;

	UFUNCTION(BlueprintCallable)
	virtual void HandleButtonHover();

	UFUNCTION(BlueprintCallable)
	virtual void HandleButtonClick();
};
