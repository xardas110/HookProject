#include "SkillTree.h"
#include "../PlayerBase.h"
#include "../../Helpers/Macros.h"

#define CreateAbility(Ability) \
	Ability = CreateDefaultSubobject<UAbilityBase>(TEXT(#Ability)); \
	AbilityMap.Add(FString(#Ability), Ability);

#define UpdateAbility(Ability) \
	AbilityMap[#Ability] = Ability

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
	
	GrappleSpeedUpgrade->Init(SkillTreeWidget->GrappleSpeedButton, this, &PlayerBase->HookRetractMaxSpeed);
	SwordKnockBackUpgrade->Init(SkillTreeWidget->SwordKnockBackButton, this, &PlayerBase->SwordPtr->KnockBackValue);
	SwordLengthUpgrade->Init(SkillTreeWidget->SwordLengthButton, this, &PlayerBase->SwordPtr->SwordLength);
	
	UpdateAbility(GrappleSpeedUpgrade);
	UpdateAbility(SwordKnockBackUpgrade);
	UpdateAbility(SwordLengthUpgrade);
}

bool USkillTree::AreSkillsMaxed()
{
	for (const auto Ability : AbilityMap)
		if (Ability.Value->CurrentRank != Ability.Value->MaxRank)
			return false;

	return true;
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
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->OnPlayerLevelUp();
	}
}

void USkillTree::ApplyUpgrades()
{
	for (const auto Ability : AbilityMap)
		Ability.Value->ApplyAbilityUpgrade();
}

void USkillTree::IncrementSkillPoints()
{
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
	CalculateNextLevelExperience();
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
