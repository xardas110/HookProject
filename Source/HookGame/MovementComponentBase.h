// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "MovementComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOOKGAME_API UMovementComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovementComponentBase();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DebugLocation();

private:
	
	//Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float DelayBeforeReturn = 20.0f;

	//Vectors
	FVector InitialLocation;
	FVector NewLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	FVector TargetLocation = { 1.0f,1.0f,1.0f };

	//Movable?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	bool Movable = false;

	//Stop movement
	bool StopMovement = false;

		
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};