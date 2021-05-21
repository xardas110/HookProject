#include "AbilityBase.h"
#include "Components/Button.h"
#include "SkillTree.h"
#include "../../Helpers/Macros.h"

UAbilityBase::UAbilityBase()
{
}

void UAbilityBase::Init(UButton* ButtonRef, USkillTree* SkillTreeRef, float* ValuePtr)
{	
	ReturnIfNull(SkillTreeRef);
	ReturnIfNull(SkillTreeRef->SkillTreeToolTipWidget);
	ReturnIfNull(ButtonRef);
	
	this->SkillTreePtr = SkillTreeRef;
	this->Button = ButtonRef;
	this->Base = *ValuePtr;
	this->PtrToVal = ValuePtr;
	
	ApplyAbilityUpgrade();
	
	this->Button->SetToolTip(SkillTreePtr->SkillTreeToolTipWidget);
	this->Button->OnClicked.AddDynamic(this, &UAbilityBase::HandleButtonClick);
	this->Button->OnHovered.AddDynamic(this, &UAbilityBase::HandleButtonHover);
}

void UAbilityBase::ApplyAbilityUpgrade()
{
	if (this->CurrentRank)
	{
		*PtrToVal = Base * (UpgradeRatio * CurrentRank + 1.f);
	}
}

bool UAbilityBase::CanRankUp() const
{
	return CurrentRank < MaxRank;
}

void UAbilityBase::HandleButtonHover()
{
	ReturnIfNull(SkillTreePtr);

	SkillTreePtr->SetToolTipTitle(Title);

	SkillTreePtr->SetToolTipRank(FString::Printf(TEXT("Rank %i/%i"), CurrentRank, MaxRank));

	SkillTreePtr->SetToolTipText(ToolTip);
}

void UAbilityBase::HandleButtonClick()
{
	ReturnIfNull(SkillTreePtr);

	ReturnIfNull(CanRankUp());

	ReturnIfNull(SkillTreePtr->HasEnoughSkillPoints());

	CurrentRank++;

	ApplyAbilityUpgrade();

	SkillTreePtr->SetToolTipRank(FString::Printf(TEXT("Rank %i/%i"), CurrentRank, MaxRank));
	SkillTreePtr->DecrementSkillPoints();
}