// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeToolTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API USkillTreeToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ToolTipTextSkillTree;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RankText;

	void SetTitleText(const FString& Txt);
	void SetToolTipText(const FString& Txt);
	void SetRankText(const FString& Txt);
};
