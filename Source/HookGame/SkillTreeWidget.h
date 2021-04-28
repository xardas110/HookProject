// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void OnCreationFromPalette() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* GrappleSpeedButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SwordKnockBackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SwordLengthButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Title;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelUpText;
};