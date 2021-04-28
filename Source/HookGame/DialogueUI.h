// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dialogue.h"
#include "Subtitles.h"
#include "DialogueUI.generated.h"


UCLASS()
class HOOKGAME_API UDialogueUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FString DisplayedSubtitle;

	UFUNCTION(BlueprintCallable, Category = DialogSystem)
	void UpdateSubtitles(TArray<FSubtitle> Subtitles);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString> QuestionsForHUD;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = DialogueSystem)
	void Show();

	UPROPERTY(BlueprintReadWrite, Category = NPCFollow)
	bool bNPCFollowPlayer = false;
	
};

class UMGAsyncTask : public FNonAbandonableTask
{
	/*The subtitles that we're going to display in the game*/
	TArray<FSubtitle> Subs;

	/*A reference to the UI*/
	UDialogueUI* DialogUI;

public:

	
	UMGAsyncTask(TArray<FSubtitle>& Subs, UDialogueUI* DialogUI)
	{
		this->Subs = Subs;
		this->DialogUI = DialogUI;
	}

	/*Function needed by the UE in order to determine what's the tasks' status*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMGAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	/*This function executes each time this thread is active - UE4 searches for a function named DoWord() and executes it*/
	void DoWork()
	{
		for (int32 i = 0; i < Subs.Num(); i++)
		{
			//Sleep means that we pause this thread for the given time
			//FPlatformProcess::Sleep(Subs[i].SubtitleDisplayTime);

			//Update our subtitles after the thread comes back
			DialogUI->DisplayedSubtitle = Subs[i].Subtitle;
			//Sleep 1 second to let the user read the text
			FPlatformProcess::Sleep(Subs[i].SubtitleDisplayTime);
		}


		//Clear the subtitle
		DialogUI->DisplayedSubtitle = FString("");
	}
};
