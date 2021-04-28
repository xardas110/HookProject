// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Subtitles.h"
#include "Dialogue.generated.h"

USTRUCT(BlueprintType)
struct FDialogue : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString QuestionText;

	UPROPERTY(EditAnywhere)
	USoundBase* DialogueSoundEffect;

	UPROPERTY(EditAnywhere)
	TArray<FSubtitle> Subtitles;

	UPROPERTY(EditAnywhere)
	bool bShouldAIRespond;
	
};