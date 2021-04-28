// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Subtitles.generated.h"

USTRUCT(BlueprintType)
struct FSubtitle : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	//The subtitle that will be displayed for x amount of time in the UI.
	UPROPERTY(EditAnywhere)
	FString Subtitle;

	UPROPERTY(EditAnywhere)
	float SubtitleDisplayTime;

};