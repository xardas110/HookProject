#include "MainHUD.h"
#include "../PlayerBase.h"
#include "../../Helpers/Macros.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UMainHUD::SetHintText(const FString& HintText)
{
	HintTextBlock->SetText(FText::FromString(HintText));
}

void UMainHUD::Init()
{
	LevelUpButton->OnClicked.AddDynamic(this, &UMainHUD::OpenSkillTree);
	InitSubtitle();
}

void UMainHUD::HideLevelUpButton()
{
	LevelUpButton->SetVisibility(ESlateVisibility::Hidden);
	LevelUpIcon->SetVisibility(ESlateVisibility::Hidden);
	LevelUpTextIcon->SetVisibility(ESlateVisibility::Hidden);
	LevelUpText->SetVisibility(ESlateVisibility::Hidden);
	StopAnimation(LevelUpAnimation);
}

void UMainHUD::ShowLevelUpButton()
{
	LevelUpButton->SetVisibility(ESlateVisibility::Visible);
	LevelUpIcon->SetVisibility(ESlateVisibility::Visible);
	LevelUpTextIcon->SetVisibility(ESlateVisibility::Visible);
	LevelUpText->SetVisibility(ESlateVisibility::Visible);
	
	if (!IsAnimationPlaying(LevelUpAnimation))
	{
		PlayAnimation(LevelUpAnimation, 0, 10000, EUMGSequencePlayMode::Forward, 1, true);
	}
}

void UMainHUD::SetQuestionText(UTextBlock* Question, const FString& QuestionText)
{
	Question->SetText(FText::FromString(QuestionText));
}

void UMainHUD::UpdateQuestionText()
{
	ReturnIfNull(QuestionsForHUD.Num());
	SetQuestionText(Question1, QuestionsForHUD[0]);
	SetQuestionText(Question2, QuestionsForHUD[1]);
	SetQuestionText(Question3, QuestionsForHUD[2]);
	SetQuestionText(Question4, QuestionsForHUD[3]);
	SetQuestionText(Question5, QuestionsForHUD[4]);
	SetQuestionText(Question6, QuestionsForHUD[5]);
}

void UMainHUD::InitSubtitle()
{
	ButtonQuestion1->OnClicked.AddDynamic(this, &UMainHUD::OnButton1Pressed);
	ButtonQuestion2->OnClicked.AddDynamic(this, &UMainHUD::OnButton2Pressed);
	ButtonQuestion3->OnClicked.AddDynamic(this, &UMainHUD::OnButton3Pressed);
	ButtonQuestion4->OnClicked.AddDynamic(this, &UMainHUD::OnButton4Pressed);
	ButtonQuestion5->OnClicked.AddDynamic(this, &UMainHUD::OnButton5Pressed);
	ButtonQuestion6->OnClicked.AddDynamic(this, &UMainHUD::OnButton6Pressed);
}

void UMainHUD::OnButton1Pressed()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->TalkWithNPC("Q1");
		ButtonQuestion1->SetVisibility(ESlateVisibility::Hidden);
		Question1->SetVisibility(ESlateVisibility::Hidden);
	}
	UE_LOG(LogTemp, Warning, TEXT("Button has been pressed"));
}

void UMainHUD::OnButton2Pressed()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->TalkWithNPC("Q2");
		ButtonQuestion2->SetVisibility(ESlateVisibility::Hidden);
		Question2->SetVisibility(ESlateVisibility::Hidden);
	}
	UE_LOG(LogTemp, Warning, TEXT("Button has been pressed"));
}

void UMainHUD::OnButton3Pressed()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->TalkWithNPC("Q3");
		ButtonQuestion3->SetVisibility(ESlateVisibility::Hidden);
		Question3->SetVisibility(ESlateVisibility::Hidden);
	}
	UE_LOG(LogTemp, Warning, TEXT("Button has been pressed"));
}

void UMainHUD::OnButton4Pressed()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->TalkWithNPC("Q4");
		PlayerBase->bNPCFollowPlayer = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Button has been pressed"));
}

void UMainHUD::OnButton5Pressed()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->TalkWithNPC("Q5");
		PlayerBase->bNPCFollowPlayer = false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Button has been pressed"));
}

void UMainHUD::OnButton6Pressed()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
	{
		PlayerBase->HideDialogueUI();
		HintBorder->SetVisibility(ESlateVisibility::Hidden);
		HintTextBlock->SetVisibility(ESlateVisibility::Hidden);
		PlayerBase->bPlayerIsTalking = false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Button has been pressed"));
}

void UMainHUD::OpenSkillTree()
{
	const auto PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(const auto PlayerBase = Cast<APlayerBase>(PlayerReference))
		PlayerBase->OnSkillTreePressed();	
}

