#include "HintWidget.h"
#include "Components/TextBlock.h"

void UHintWidget::SetHintText(const FString& HintText)
{
	HintTextBlock->SetText(FText::FromString(HintText));
}
