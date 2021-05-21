#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "MainHUD.generated.h"

UCLASS()
class HOOKGAME_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()
		friend class APlayerBase;

	//Hints
	
	UPROPERTY(meta = (BindWidget))
		UTextBlock* HintTextBlock;

	UPROPERTY(meta = (BindWidget))
		UBorder* HintBorder;

	void SetHintText(const FString& HintText);
	
	//Checkpoint

	UPROPERTY(meta = (BindWidget))
		class UImage* CheckpointIcon;

	UPROPERTY(meta = (BindWidget))
		class UImage* CheckpointText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* CheckPointAnimation;

	//LevelUpMessage

	UPROPERTY(meta = (BindWidget))
		class UImage* LevelUpIcon;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* LevelUpButton;

	UPROPERTY(meta = (BindWidget))
		class UImage* LevelUpTextIcon;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* LevelUpText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* LevelUpAnimation;

	void Init();

	void HideLevelUpButton();
	void ShowLevelUpButton();
	//DialogueSystem
	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonQuestion1;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonQuestion2;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonQuestion3;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonQuestion4;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonQuestion5;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonQuestion6;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Question1;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Question2;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Question3;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Question4;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Question5;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Question6;
	
	void SetQuestionText(UTextBlock* Question, const FString& QuestionText);

	void UpdateQuestionText();

	UFUNCTION()
	void InitSubtitle();

	UFUNCTION()
	void OnButton1Pressed();
	UFUNCTION()
	void OnButton2Pressed();
	UFUNCTION()
	void OnButton3Pressed();
	UFUNCTION()
	void OnButton4Pressed();
	UFUNCTION()
	void OnButton5Pressed();
	UFUNCTION()
	void OnButton6Pressed();

	UPROPERTY()
	TArray<FString> QuestionsForHUD;
	
public:
	UFUNCTION()
	void OpenSkillTree();
};
