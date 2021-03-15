// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UCLASS()
class HOOKGAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	virtual void BeginPlay() override;
public:

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
