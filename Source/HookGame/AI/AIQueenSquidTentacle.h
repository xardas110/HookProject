// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviorBase.h"
#include "AIQueenSquidTentacle.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIQueenSquidTentacle : public AAIBehaviorBase
{
	GENERATED_BODY()

	AAIQueenSquidTentacle();
public:
		///		Constructor Values		///

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* BossPlatform{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCapsuleComponent* AttackHitbox{ nullptr };

	///			Variables			///

	UPROPERTY(EditAnywhere, Category = "Boss Mechanics")
		bool bAlwaysAttacking = false;  // Used for the extra tentacle platform earlier in the level. Makes the tentacle platform constantly attack
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Mechanics")
		bool bPlatformIsWalkable = true; // If the platform is walkable. Has a tentacle slapped this platform, it is not walkable.
	
	UPROPERTY(EditAnywhere, Category = "Boss Mechanics")
		float PlatformFallSpeed = 1000.f; // How fast the platform moves down when hit by a tentacle
	
	UPROPERTY(EditAnywhere, Category = "Boss Mechanics")
		float PlatformFallDistance = 500.f; // How far does the platform sink when it is hit by a tentacle?
	
	UPROPERTY(EditAnywhere, Category = "Boss Mechanics")
		float TentacleDamage = 25.f; // The damage dealt to the player if they are hit by a tentacle-slap
	
	UPROPERTY(EditAnywhere, Category = "Solo Platform")
		float TimeUntilAttack = 20.f; // For the solo platform not connected to the boss fight, how ofter does the tentacle attack?
	
	float TimeSinceAttack = 0.f;
	bool bHasAttacked = false;
	FVector PlatformTarget{0.f,0.f,0.f}; // Target location of the platform when it is walkable
	FVector PlatformDownedTarget{0.f,0.f,0.f}; // Target location of the platform when a tentacle hits it down.

	///			Functions		///

	virtual void BeginPlay() override;
	virtual void Attack() override;
	virtual void Tick(float DeltaSeconds) override;
	void RespawnPlatform(float DeltaSeconds); // When called, Respawns the platform to the PlatformTarget Location
	void TentacleDrownPlatform(float DeltaTime); // When called, Moves the tentacle and platform to the PlatformDownedTarget Location
	void SetStartValues();
	void PlatformStatus(float DeltaSeconds); // Checks if this is a solo platform which always attacks, if it is currently walkable or not and calls RespawnPlatform and TentacleDrownPlatform when needed
	virtual void HandleDeath() override;
	
	UFUNCTION(BlueprintCallable)
		void TentacleBeginAttackDown(); // Called when the tentacle actually slaps the platform, Enables overlap events
	
	UFUNCTION(BlueprintCallable)
		void PlatformHitByTentacle(); // Called when the platform is hit by a tentacle, sinks the platform in water
	
	UFUNCTION(BlueprintCallable)
	virtual void EndAttack() override; // Ends the attack. Resets bool values that relate to the attack.
	
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); // Called when the tentacle overlaps the player	
};
