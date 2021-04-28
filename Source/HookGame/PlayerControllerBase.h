// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "HUD")
		TSubclassOf<UUserWidget> HUDClass;
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY()
		UUserWidget* HUD;
	
};
