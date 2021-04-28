// Fill out your copyright notice in the Description page of Project Settings.


#include "AIRobotSidekick.h"
#include "HookGameInstance.h"
#include "PlayerBase.h"

AAIRobotSidekick::AAIRobotSidekick()
{
	PropellerBlade = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Propeller Blade"));
	PropellerBlade->SetupAttachment(RootComponent);

	InteractionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	InteractionComp->SetupAttachment(RootComponent);

	
}

void AAIRobotSidekick::BeginPlay()
{
	Super::BeginPlay();

	const auto GameInstance = Cast<UHookGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		if (GameInstance->bNPCDataLoad)
		{
			GameInstance->LoadNPCData(this);
			GameInstance->bNPCDataLoad = false;
			
		}
		if (GameInstance->bNPCCheckpointDataLoad)
		{
			GameInstance->LoadNPCCheckpoint(this);
			GameInstance->bNPCCheckpointDataLoad = false;
		}
	}

	if (bInteractable)
	{
		bFollowPlayer = false;
		InteractionComp->OnComponentBeginOverlap.AddDynamic(this, &AAIRobotSidekick::InteractionActive);
		InteractionComp->OnComponentEndOverlap.AddDynamic(this, &AAIRobotSidekick::InteractionNotActive);
	}

	if (!bInteractable)
	{
		bFollowPlayer = true;

	}
	
}


void AAIRobotSidekick::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

float AAIRobotSidekick::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	UE_LOG(LogTemp, Error, TEXT(" Robots feel no pain! "));
	return 0;
}

void AAIRobotSidekick::Talk(USoundBase* SFX, TArray<FSubtitle> Subs)
{
	APlayerBase* Char = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));


	//***Uncomment this when/if we have audio***
	//
	//AudioComp->SetSound(SFX);
	//AudioComp->Play();

	//Tell the UI to update with the new subtitles
	Char->GetUI()->UpdateSubtitles(Subs);
}

void AAIRobotSidekick::InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerBase>())
	{
		if(bInteractable)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC registered that you are in the interaction area"));
			APlayerBase* Char = Cast<APlayerBase>(OtherActor);
			Char->SetTalkRangeStatus(true);
			Char->GeneratePlayerLines(PlayerLines);
			Char->SetAssociatedPawn(this);
			Char->SetInteractableNPC(this);
			Char->DisplayInteractMessage();
		}

	}

}

void AAIRobotSidekick::InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	
	if (OtherActor->IsA<APlayerBase>())
	{
		if(bInteractable)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC registered that you are no longer in the interaction area"));
			APlayerBase* Char = Cast<APlayerBase>(OtherActor);
			Char->SetTalkRangeStatus(false);
			Char->SetAssociatedPawn(nullptr);
			Char->SetInteractableNPC(nullptr);
			SetFollowStatus(Char->bNPCFollowPlayer);

			bool PlayerBaseFollowStatus = Char->bNPCFollowPlayer;

			if(Char->bNPCFollowPlayer)
			{
				bInteractable = false;
			}

			UE_LOG(LogTemp, Warning, TEXT("PlayerBaseFollowStatus is set to: %s"), (PlayerBaseFollowStatus ? TEXT("true") : TEXT("false")));
			UE_LOG(LogTemp, Warning, TEXT("NPCFollowStatus is set to: %s"), (bFollowPlayer ? TEXT("true") : TEXT("false")));
			UE_LOG(LogTemp, Warning, TEXT("NPCinteractStatus is set to: %s"), (bInteractable ? TEXT("true") : TEXT("false")));
			
		}
		
	}
}


void AAIRobotSidekick::AnswerPlayerCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay)
{
	if (!NPCLines) return;

	FString ContextString;
	FDialogue* Dialogue = NPCLines->FindRow<FDialogue>(PlayerLine, ContextString);

	APlayerBase* MainChar = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Dialogue && MainChar)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("Talk"), Dialogue->DialogueSoundEffect, Dialogue->Subtitles);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);
	}
}

void AAIRobotSidekick::SetFollowStatus(bool FollowStatus)
{

	if(FollowStatus == true)
	{
		bInteractable = false;
		bFollowPlayer = true;
	}
	else
	{
		bInteractable = true;
		bFollowPlayer = false;
	}
	
}
