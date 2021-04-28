// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeToolTipWidget.h"

#include "Components/TextBlock.h"

void USkillTreeToolTipWidget::SetTitleText(const FString& Txt)
{
	TitleText->SetText(FText::FromString(Txt));
}

void USkillTreeToolTipWidget::SetToolTipText(const FString& Txt)
{
	ToolTipTextSkillTree->SetText(FText::FromString(Txt));
}

void USkillTreeToolTipWidget::SetRankText(const FString& Txt)
{
	RankText->SetText(FText::FromString(Txt));
}
