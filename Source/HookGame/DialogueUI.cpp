// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueUI.h"

void UDialogueUI::UpdateSubtitles(TArray<FSubtitle> Subtitles)
{

		if (!Subtitles.IsValidIndex(0)) return;

		//Start a background task in a low priority thread
		(new FAutoDeleteAsyncTask<UMGAsyncTask>(Subtitles, this))->StartBackgroundTask();

	
}

