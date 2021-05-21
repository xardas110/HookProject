#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityBase.h"
#include "../Widgets/SkillTreeWidget.h"
#include "../Widgets/SkillTreeToolTipWidget.h"
#include "SkillTree.generated.h"

class UCanvasPanel;

#define LogIfFailed(name) if (!name) if (!PrintLine(#name))

#define CreateAndAddChildToMainCanvas(MainCanvas, Child, ChildType, CastSlot) \
Child = CreateDefaultSubobject<ChildType>(TEXT(#Child)); \
ReturnIfNull(Child); \
MainCanvas->AddChild(Child); \
ReturnIfNull(Child->Slot); \
auto Child##Slot = Cast<CastSlot>(Child->Slot);

#define AddChildToMainCanvas(MainCanvas, Child, ChildType, CastSlot) \
ReturnIfNull(Child); \
MainCanvas->AddChild(Child); \
ReturnIfNull(Child->Slot); \
auto Child##Slot = Cast<CastSlot>(Child->Slot);

UCLASS( ClassGroup=(Custom))
class HOOKGAME_API USkillTree : public UActorComponent
{
	GENERATED_BODY()

	friend class UAbilityBase;
public:	
	USkillTree();

	UPROPERTY()
	USkillTreeWidget* SkillTreeWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USkillTreeWidget> SkillTreeWidgetClass;

	UPROPERTY()
	USkillTreeToolTipWidget* SkillTreeToolTipWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USkillTreeToolTipWidget> SkillTreeToolTipWidgetClass;

	UPROPERTY(EditAnywhere)
	UAbilityBase* GrappleSpeedUpgrade;

	UPROPERTY(EditAnywhere)
	UAbilityBase* SwordKnockBackUpgrade;

	UPROPERTY(EditAnywhere)
	UAbilityBase* SwordLengthUpgrade;

	bool AreSkillsMaxed();
	
	UPROPERTY()
	uint32 Level{ 0 };
	UPROPERTY()
	uint32 CurrentExperience{ 0 };
	UPROPERTY()
	uint32 NextExperience{ 0 };
	
	void CalculateNextLevelExperience();

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	uint32 SkillPoints{ 0 };
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	uint32 ReqPtsForSkillUp {1};
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	uint32 SkillPointsPrLevel {2};
	
	void Init(_Inout_ class APlayerBase* Player, _Inout_ APlayerController* PlayerController);
	
	bool bShowSkillTree{ false };

	void UpdateSkillPointText();
	
	void OnSkillTreePressed();

	void IncrementLevel();

	void ApplyUpgrades();
	
	void IncrementSkillPoints();

	void DecrementSkillPoints();
	
	void AddExperience(uint32 Experience);
		
	void SetSkillTreeTitle(FString String) const;
	
	void SetToolTipTitle(FString String) const;

	void SetToolTipRank(FString String) const;

	void SetToolTipText(FString String) const;
		
	bool HasEnoughSkillPoints() const;

	void InitializeAbilities(_Inout_ class APlayerBase* Player);

	void InitWidgets(_Inout_ APlayerController* PlayerController);
	
	TMap<FString, UAbilityBase*> AbilityMap;//Must have garbage collection

private:
	UPROPERTY()
	class APlayerBase* Player{ nullptr };	
};

