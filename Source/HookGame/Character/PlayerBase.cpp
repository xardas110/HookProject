#include "PlayerBase.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Ai/AIBehaviorBase.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SceneView.h"
#include "../GameInstance/HookGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "../Components/GrappleTraceComponent.h"
#include "../Components/GrappleTracer.h"
#include "../Helpers/Macros.h"
#include "Components/Image.h"






	PlayerHUD->HintBorder->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->HintTextBlock->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->CheckpointIcon->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->CheckpointText->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->LevelUpButton->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->LevelUpIcon->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->LevelUpTextIcon->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->LevelUpText->SetVisibility(ESlateVisibility::Hidden);


	//Dialogue
	PlayerHUD->ButtonQuestion1->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion2->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion3->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion4->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion5->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion6->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question1->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question2->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question3->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question4->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question5->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question6->SetVisibility(ESlateVisibility::Hidden);
	


	

	
	ReturnIfNull(PlayerHUD);
	PlayerHUD->AddToViewport();



void APlayerBase::InteractInit(APlayerControllerBase* PlayerController)
{
	ReturnIfNull(InteractionPopup);

	InteractionWidget = CreateWidget(PlayerController, InteractionPopup);
	ReturnIfNull(InteractionWidget);

	InteractionWidget->AddToViewport();
	InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
}



void APlayerBase::InteractLoad()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::InteractionActive);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerBase::InteractionNotActive);

	bPlayerIsTalking = false;
	PlayerCanInteract = false;
	AssociatedNPC = nullptr;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

}


void APlayerBase::LoadPlayerData()
{
	const auto GameInstance = Cast<UHookGameInstance>(GetGameInstance());
	ReturnIfNull(GameInstance);

	if(GameInstance->bLevelIsLoaded)
	{
		GameInstance->bLevelIsLoaded = false;
		GameInstance->LoadPlayerData(this);
	}

	if(GameInstance->bCheckpointIsLoaded)
	{
		GameInstance->bCheckpointIsLoaded = false;
		GameInstance->LoadPlayerCheckpointData(this);
	}
}






void APlayerBase::SaveGame()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->SaveGameData(this);
}

void APlayerBase::SaveLatestCheckpoint()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->SaveLatestCheckpoint(this);
}
void APlayerBase::LoadCheckpoint()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->LoadLatestCheckpoint(this);
}

void APlayerBase::LoadGame()
{
	UHookGameInstance* GameInstanceRef = Cast<UHookGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ReturnIfNull(GameInstanceRef);
	GameInstanceRef->LoadGameData(this);
}


float APlayerBase::GetCurrentHealth() const
{
	return CurrentHealth;
}

float APlayerBase::GetMaxHealth() const
{
	return MaxHealth;
}

void APlayerBase::InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && (OtherActor->IsA(AAIRobotSidekick::StaticClass())))
	{
		const auto RobotRef = Cast<AAIRobotSidekick>(OtherActor);
		if(RobotRef->bInteractable)
		{
#ifdef _DEBUG_INTERACTION
			UE_LOG(LogTemp, Warning, TEXT("Player is now in the interaction area and can interact"));
#endif
			PlayerCanInteract = true;
		}
	}
}

void APlayerBase::InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	if (PlayerCanInteract)
	{
		PlayerCanInteract = false;
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("Player is not in the interaction area anymore"));
#endif
	}
}

void APlayerBase::GeneratePlayerLines(UDataTable* PlayerLines2)
{
	ReturnIfNull(PlayerLines2);
	TArray<FName> PlayerOptions = PlayerLines2->GetRowNames();

	for (auto const It : PlayerOptions)
	{
		FDialogue* Dialog = RetrieveDialogue(PlayerLines2, It);

		if (Dialog)
		{
			Questions.Add(Dialog->QuestionText);
		}
	}
	PlayerHUD->QuestionsForHUD = Questions;
	PlayerHUD->UpdateQuestionText();
	AvailableLines = PlayerLines;
}


void APlayerBase::SetInteractableNPC(AAIRobotSidekick* NPC)
{	
	InteractableNPC = NPC;	
	if(InteractableNPC)
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	else
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);	
}

void APlayerBase::DisplayInteractMessage()
{
#ifdef _DEBUG_INTERACTION
	UE_LOG(LogTemp, Warning, TEXT("DisplayInteractMessage"));
#endif
	const auto PlayerControllerReference = Cast<APlayerControllerBase>(Controller);
	ReturnIfNull(PlayerControllerReference);	
	ReturnIfNull(InteractionWidget);
	ReturnIfNull(PlayerCanInteract);
						
	if(InteractableNPC && !bNPCFollowPlayer)
	{
		NPCPosition = InteractableNPC->GetActorLocation();
	}
				
	FVector2D PositionInViewport;
	PlayerControllerReference->ProjectWorldLocationToScreen(NPCPosition, PositionInViewport);
	
	const FVector2D SizeOfInteractHUD{ InteractSizeX,InteractSizeY };
	InteractionWidget->SetDesiredSizeInViewport(SizeOfInteractHUD);
	PositionInViewport.X -= 50;
	PositionInViewport.Y -= 90;
	InteractionWidget->SetPositionInViewport(PositionInViewport);										
}

void APlayerBase::ToolTipTextUpdate(float DeltaTime)
{
	if (!ToolTipTextArray.Num())
	{
		return;
	}
	
	ToolTipTimer += DeltaTime;

	float Duration = ToolTipTextArray[0].Key;

	if(PlayerHUD->HintTextBlock->GetVisibility() == ESlateVisibility::Hidden && PlayerHUD->HintBorder->GetVisibility() == ESlateVisibility::Hidden)
	{
		PlayerHUD->SetHintText(ToolTipTextArray[0].Value);
		PlayerHUD->HintTextBlock->SetVisibility(ESlateVisibility::Visible);
		PlayerHUD->HintBorder->SetVisibility(ESlateVisibility::Visible);
	}
	
	if(ToolTipTimer > Duration)
	{
		ToolTipTextArray.RemoveAt(0);
		if(PlayerHUD->HintTextBlock->GetVisibility()==ESlateVisibility::Visible)
		{
			PlayerHUD->HintTextBlock->SetVisibility(ESlateVisibility::Hidden);
			PlayerHUD->HintBorder->SetVisibility(ESlateVisibility::Hidden);
		}
		ToolTipTimer = 0.f;
	}
	
	
}

void APlayerBase::CheckPointWidget(float DeltaTime)
{
	if(!bDisplayCheckWidget)
	{
		return;
	}
	
	CheckPointTimer += DeltaTime;
		
	if (PlayerHUD->CheckpointIcon->GetVisibility() == ESlateVisibility::Hidden && PlayerHUD->CheckpointText->GetVisibility() == ESlateVisibility::Hidden && bDisplayCheckWidget)
	{
		PlayerHUD->CheckpointIcon->SetVisibility(ESlateVisibility::Visible);
		PlayerHUD->CheckpointText->SetVisibility(ESlateVisibility::Visible);
		PlayerHUD->PlayAnimation(PlayerHUD->CheckPointAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
	}

	if (CheckpointWidgetDisplayTime < CheckPointTimer)
	{
		CheckPointTimer = 0.f;
		if(PlayerHUD->CheckpointIcon->GetVisibility() == ESlateVisibility::Visible && PlayerHUD->CheckpointText->GetVisibility() == ESlateVisibility::Visible)
		{
			PlayerHUD->CheckpointIcon->SetVisibility(ESlateVisibility::Hidden);
			PlayerHUD->CheckpointText->SetVisibility(ESlateVisibility::Hidden);
		}
		bDisplayCheckWidget = false;
	}
}

void APlayerBase::ShowDialogueUI()
{
	PlayerHUD->ButtonQuestion1->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->ButtonQuestion2->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->ButtonQuestion3->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->ButtonQuestion4->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->ButtonQuestion5->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->ButtonQuestion6->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->Question1->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->Question2->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->Question3->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->Question4->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->Question5->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->Question6->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->HintTextBlock->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->HintBorder->SetVisibility(ESlateVisibility::Visible);
}

void APlayerBase::HideDialogueUI()
{
	PlayerHUD->ButtonQuestion1->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion2->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion3->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion4->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion5->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->ButtonQuestion6->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question1->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question2->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question3->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question4->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question5->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->Question6->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUD->HintTextBlock->SetVisibility(ESlateVisibility::Visible);
	PlayerHUD->HintBorder->SetVisibility(ESlateVisibility::Visible);
}

void APlayerBase::TalkWithNPC(FName index)
{
	bTalkingWithNPC = true;
	bPlayerIsCurrentlyTalking = true;
	CurrentIndex = index;
	PlayerTalkTimer = 0.f;
	RobotTalkTimer = 0.f;
}

void APlayerBase::UpdateTalking(float DeltaTime)
{
	if(bTalkingWithNPC)
	{
		PlayerTalkTimer += DeltaTime;
		
		
		FString ContextString;
		FDialogue* PlayerDialogue = PlayerLines->FindRow<FDialogue>(CurrentIndex, ContextString, true);
		FDialogue* NPCDialogue = NPCLines->FindRow<FDialogue>(CurrentIndex, ContextString, true);
		
		ReturnIfNull(PlayerDialogue);
		ReturnIfNull(NPCDialogue);
		
		PlayerHUD->SetHintText(PlayerDialogue->Subtitles[0].Subtitle);
		
		if (PlayerTalkTimer < PlayerDialogue->Subtitles[0].SubtitleDisplayTime)
			return;

		if (AllLinesCurrentIndex >= NPCDialogue->Subtitles.Num())
		{
			bPlayerIsCurrentlyTalking = false;
			bTalkingWithNPC = false;
			PlayerHUD->SetHintText("");
			AllLinesCurrentIndex = 0;
			return;
		}
		
		PlayerHUD->SetHintText(NPCDialogue->Subtitles[AllLinesCurrentIndex].Subtitle);

		RobotTalkTimer += DeltaTime;
		
		if (NPCDialogue->Subtitles[AllLinesCurrentIndex].SubtitleDisplayTime < RobotTalkTimer)
		{
			AllLinesCurrentIndex++;
			RobotTalkTimer = 0.f;
		}
	}
}

void APlayerBase::InteractWithNPC()
{
	ReturnIfNull(PlayerCanInteract);
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("You pressed E to interact with the NPC"));
#endif
	bPlayerIsTalking = !bPlayerIsTalking;
	ToggleUI();
	if (bPlayerIsTalking && AssociatedNPC)
	{
		const FVector Location = AssociatedNPC->GetActorLocation();
		const FVector TargetLocation = GetActorLocation();

		AssociatedNPC->SetActorRotation((TargetLocation - Location).Rotation());
	}
}

void APlayerBase::ToggleUI()
{
	const auto PlayerControllerRef = Cast<APlayerControllerBase>(Controller);
	if (!PlayerControllerRef) //Don't macro this.
		return;
		
	if(bPlayerIsTalking)
	{
#ifdef _DEBUG_INTERACTION
		UE_LOG(LogTemp, Warning, TEXT("PlayerisNowTalking"));
#endif
		if(!bPlayerIsTalking)
		{				
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerControllerRef);
			PlayerControllerRef->bShowMouseCursor = true;
			PlayerControllerRef->bEnableClickEvents = true;
			PlayerControllerRef->bEnableClickEvents = true;
			PlayerControllerRef->bEnableMouseOverEvents = true;
			PlayerControllerRef->SetMouseCursorWidget(EMouseCursor::Crosshairs, WidgetCursorNotLocked);
			HideDialogueUI();
#ifdef _DEBUG_INTERACTION
			UE_LOG(LogTemp, Warning, TEXT("DialogueUIHUDRemoved"));
#endif
		}	
		else
		{				
			ShowDialogueUI();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUI(PlayerControllerRef);
			PlayerControllerRef->bShowMouseCursor = true;
			PlayerControllerRef->bEnableClickEvents = true;
			PlayerControllerRef->bEnableMouseOverEvents = true;	
		}		
	}else
	{	
		HideDialogueUI();
	}		
}

FDialogue* APlayerBase::RetrieveDialogue(UDataTable* TableToSearch, FName RowName)
{
	if (!TableToSearch)
	{
		return nullptr;
	}
	FString ContextString; 
	return TableToSearch->FindRow<FDialogue>(RowName, ContextString);
}