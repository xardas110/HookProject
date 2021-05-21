#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HintWidget.generated.h"

UCLASS()
class HOOKGAME_API UHintWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HintTextBlock;

	void SetHintText(const FString& HintText);	
};
