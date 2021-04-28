// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase.h"
#include "AISquidbirdLeaperController.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAISquidbirdLeaperController : public AAIControllerBase
{
	GENERATED_BODY()
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
};
