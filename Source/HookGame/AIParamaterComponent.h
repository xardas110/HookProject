// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIParamaterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOOKGAME_API UAIParamaterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIParamaterComponent();

	UPROPERTY(EditAnywhere, Category = "Movement")
		bool bPatrollingEnabled = false; // If set to true, the actor will patrol with the following variables. If set to false, it will not have a patrolling system.

	UPROPERTY(EditAnywhere, Category="Movement")
		FVector PatrolDistance { 0.f,0.f,0.f }; // Distance the actor will move from their start location when patrolling

	UPROPERTY(EditAnywhere, Category = "Movement")
		float PatrolEndTurnAngle = 180.f; // How much the actor will turn after they have moved their patrol distance. Default is to turn around completely and return to start location.
	FRotator PatrolEndTurnAngleRotation{ 0.f,0.f,PatrolEndTurnAngle };

	UPROPERTY(EditAnywhere, Category = "Movement")
		float PatrolEndTurnSpeed = 0.5f; // The speed the actor turns upon finishing their patrol distance, value kept between 0-1.
	UPROPERTY(EditAnywhere, Category = "Behavior")
		float DetectionRange = 100.f; // Max range the actor will react to the player

	UPROPERTY(EditAnywhere, Category = "Behavior")
		float ResetTimer = 4.f; // Time before actor resets, and returns to start position

	bool bPlayerInRange = false;
	bool bReturnToStart = true;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
