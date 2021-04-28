
#include "SkillTree.h"
#include <cassert>
#include "PlayerBase.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/BorderSlot.h"
#include "Components/CanvasPanelSlot.h"

#define CreateAbility(Ability) \
	Ability = CreateDefaultSubobject<UAbilityBase>(TEXT(#Ability)); \
	AbilityMap.Add(FString(#Ability), Ability);

USkillTree::USkillTree()
{
	PrimaryComponentTick.bCanEverTick = false;

	CreateAbility(GrappleSpeedUpgrade);
	CreateAbility(SwordKnockBackUpgrade);
	CreateAbility(SwordLengthUpgrade);
}

void USkillTree::InitializeAbilities(APlayerBase* PlayerBase)
{
	ReturnIfNull(PlayerBase);
	
	GrappleSpeedUpgrade->InitButton(SkillTreeWidget->GrappleSpeedButton, this, &PlayerBase->HookRetractMaxSpeed);
	SwordKnockBackUpgrade->InitButton(SkillTreeWidget->SwordKnockBackButton, this, &PlayerBase->SwordPtr->KnockBackValue);
	SwordLengthUpgrade->InitButton(SkillTreeWidget->SwordLengthButton, this, &PlayerBase->SwordPtr->SwordLength);
}


void USkillTree::CalculateNextLevelExperience()
{
	NextExperience = (Level * 1000) + 1000;
}

void USkillTree::OnSkillTreePressed()
{

	ReturnIfNull(SkillTreeWidget);

	bShowSkillTree = !bShowSkillTree;

	if (bShowSkillTree)
	{
		SkillTreeWidget->AddToViewport();
		UpdateSkillPointText();
	}
	else
		SkillTreeWidget->RemoveFromViewport();
}

void USkillTree::IncrementLevel()
{
	Level++;
	IncrementSkillPoints();
}

void USkillTree::IncrementSkillPoints()
{
	UE_LOG(LogTemp, Warning, TEXT("skillpoints incremented: %i"), SkillPoints);
	SkillPoints += SkillPointsPrLevel;
	UpdateSkillPointText();
}

void USkillTree::DecrementSkillPoints()
{
	SkillPoints -= ReqPtsForSkillUp;
	UpdateSkillPointText();
}

void USkillTree::AddExperience(uint32 Experience)
{
	UE_LOG(LogTemp, Warning, TEXT("Experience added: %i"), Experience);
	CalculateNextLevelExperience();
	UE_LOG(LogTemp, Warning, TEXT("next level experience: %i"), Experience);
	CurrentExperience += Experience;
	if (CurrentExperience >= NextExperience)
		IncrementLevel();
}

void USkillTree::SetSkillTreeTitle(FString String) const
{
	if (SkillTreeWidget)
		SkillTreeWidget->Title->SetText(FText::FromString(String));
}

void USkillTree::SetToolTipTitle(FString String) const
{
	ReturnIfNull(SkillTreeToolTipWidget);
	SkillTreeToolTipWidget->SetTitleText(String);
}

void USkillTree::SetToolTipRank(FString String) const
{
	ReturnIfNull(SkillTreeToolTipWidget);
	SkillTreeToolTipWidget->SetRankText(String);
}

void USkillTree::SetToolTipText(FString String) const
{
	ReturnIfNull(SkillTreeToolTipWidget);
	SkillTreeToolTipWidget->SetToolTipText(String);
}

bool USkillTree::HasEnoughSkillPoints() const
{
	return ReqPtsForSkillUp <= SkillPoints;
}

void USkillTree::InitWidgets(APlayerController* PlayerController)
{
	SkillTreeWidget = Cast<USkillTreeWidget>(CreateWidget(PlayerController, SkillTreeWidgetClass));
	SkillTreeToolTipWidget = Cast<USkillTreeToolTipWidget>(CreateWidget(PlayerController, SkillTreeToolTipWidgetClass));

	ReturnIfNull(SkillTreeWidget);
	ReturnIfNull(SkillTreeToolTipWidget);
}

void USkillTree::Init(_Inout_ APlayerBase* PlayerBase, _Inout_ APlayerController* PlayerController)
{
	Player = PlayerBase;
	InitWidgets(PlayerController);
	InitializeAbilities(PlayerBase);
	CalculateNextLevelExperience();
}

void USkillTree::UpdateSkillPointText()
{
	SetSkillTreeTitle(FString::Printf(TEXT("Skillpoints: %i"), SkillPoints));
}
