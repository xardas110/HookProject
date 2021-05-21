#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

UCLASS()
class HOOKGAME_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* GrappleSpeedButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SwordKnockBackButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SwordLengthButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Title;
};
