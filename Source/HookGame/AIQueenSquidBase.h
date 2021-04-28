// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviorBase.h"

#include "AIQueenSquidBase.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIQueenSquidBase : public AAIBehaviorBase
{
	GENERATED_BODY()
		AAIQueenSquidBase();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* DamageHitbox{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCapsuleComponent* LaserHitbox{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* LaserBeam{ nullptr };
	UPROPERTY(BlueprintReadWrite)
		bool bIsVulnerable = false; // is the boss vulnerable for attacks?
	UPROPERTY(BlueprintReadWrite)
		bool bTookDamage = false; // Did the boss just take damage from the player?

	///			Boss-Setup values		///

	UPROPERTY(EditAnywhere, Category = "Setup")
		float LaserDamage = 50.f;
	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<class AAIQueenSquidTentacle> TentacleBP; // Blueprint of the tentacle blueprint used for attacks, and also holds the platforms the player stands on.
	UPROPERTY(EditAnywhere,Category="Setup")
		FVector PlatformSpawnPoint{0.f,0.f,0.f}; // Spawns the first platform for the boss fight
	UPROPERTY(EditAnywhere, Category = "Setup")
		FVector PlatformSpawnOffset{0.f,0.f,0.f}; // How far each platform spawns from the last
	UPROPERTY(EditAnywhere, Category = "Setup")
		int NumberOfPlatforms = 4;  // The amount of platforms to spawn
	UPROPERTY(EditAnywhere, Category = "Setup")
		int AttacksPerRound = 3; // How many attacks before the boss inspects?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		float TimeBetweenAttacks = 2.f; // the time between each tentacle slam
	UPROPERTY(EditAnywhere, Category = "Setup")
		float InspectLowering = 1000.f; // How fast does the boss move while inspecting
	UPROPERTY(EditAnywhere, Category = "Setup")
		FVector InspectionLocation{0.f,0.f,0.f};
	UPROPERTY(EditAnywhere, Category = "Setup")
		FRotator InspectRotationStart{0.f,0.f,0.f};
	UPROPERTY(EditAnywhere, Category = "Setup")
		FRotator InspectRotationOffset{0.f,0.f,0.f};
	UPROPERTY(EditAnywhere, Category = "Setup")
		FRotator InspectRotationEnd{0.f,0.f,0.f};
	UPROPERTY(EditAnywhere, Category = "Setup")
		float InspectRotationSpeed = 5.f;

	bool bIsInspecting = false;
	
	TArray<class AAIQueenSquidTentacle*> SpawnedTentacles;

	void AttemptAttack();
	void InspectPlatforms(float DeltaTime);
	void SpawnPlatforms();
	void MoveQueen(float DeltaSeconds);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void HandleDeath() override;
	void RotateQueen(float DeltaSeconds);
	
	UFUNCTION(BlueprintCallable)	
		virtual void Attack() override;
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnLaserOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	float HowManyAttacks = 0.f; // How many tentacles have attacked this cycle?
	int SpawnedPlatforms = 0;	// How many platforms are spawned?
	bool bIsFightingPlayer = false;	// Has the Boss fight started?
	class AAIQueenSquidTentacle* AttackingTentacle{ nullptr };	// Pointer to the tentacle that is currently attacking a platform
	float DeathTimer = 0.f; // The amount of time that the boss has been dead
	float TimeUntilDespawn = 2.f; // How long after the boss dies does it respawn?
	FVector OriginalLocation{ 0.f,0.f,0.f }; // Where did the boss begin? Used for when inspecting platforms
	FRotator OriginalRotation{ 0.f,0.f,0.f };
	
	bool bHasBegunRotating = false;
	float InspectTime = 2.f; // The amount of time the boss spends inspecting the platforms after attacking
	float TimeInspected = 0.f; // The time the boss has spent since it started inspecting the platforms
};
