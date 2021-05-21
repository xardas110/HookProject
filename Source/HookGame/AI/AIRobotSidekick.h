// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviorBase.h"
#include "VisualLogger/VisualLoggerDebugSnapshotInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "../Components/Subtitles.h"
#include "AIRobotSidekick.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIRobotSidekick : public AAIBehaviorBase, public IVisualLoggerDebugSnapshotInterface
{
	GENERATED_BODY()
	
public:
	AAIRobotSidekick();

	virtual void BeginPlay() override;
	///			Constructor Values		///

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* PropellerBlade{nullptr};
	
	UPROPERTY(VisibleAnywhere)	// Dialogue system
		UCapsuleComponent* InteractionComp;
	
	UPROPERTY()
		FRotator InitialRotation = {};

	///			Variables				///

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFollowPlayer = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Robot Sidekick Parameters")
		FVector RangeToGetFrightened{0.f,0.f,0.f};  // The vision range that determines if a monster is close enough to scare the robot

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Sidekick Parameters")
		FVector MinimumDistanceToPlayer{0.f,0.f,0.f} ; // The Minimum distance the robot wants to be from the player. If under, the robot will attempt to move away

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Sidekick Parameters")
		float MaximumDistanceToPlayer= 1000.f; // The maximum distance the robot wants to be form the player. If reached, the robot will speed up to catch up with the player.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Sidekick Parameters")
		float RobotMovespeed = 550.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Sidekick Parameters")
		float CatchupSpeedMultiplier = 2.f; // The value to multiply speed with when catching up to the player


	///			Functions			///

	virtual void Tick(float DeltaTime) override;
	void SetFollowStatus(bool FollowStatus);
	void CheckDistanceToPlayer();
	void SetStartValues();
	
protected:

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
private:
	bool bRunningToCatchUp = false;

};
