// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase.h"
#include "AIQueenSquidbirdController.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIQueenSquidbirdController : public AAIControllerBase
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	void SetBlackboardValues();
};
