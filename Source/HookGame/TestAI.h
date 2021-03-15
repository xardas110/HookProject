// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestAI.generated.h"

UCLASS()
class HOOKGAME_API ATestAI : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATestAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TESTING")
		class UCapsuleComponent* MyCollider{ nullptr };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
