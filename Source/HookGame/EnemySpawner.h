// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIFlameGulper.h"
#include "AIGatlingGulper.h"
#include "AISquidbird.h"
#include "AISquidbirdLeaper.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class HOOKGAME_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Enemy blueprints")
	TSubclassOf<class AAISquidbird> SquidbirdBlueprint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy blueprints")
	TSubclassOf<class AAISquidbird> SquidbirdSprinterBlueprint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy blueprints")
	TSubclassOf<class AAISquidbirdLeaper> SquidbirddLeaperBlueprint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy blueprints")
	TSubclassOf<class AAIFlameGulper> FlamegulperBlueprint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy blueprints")
	TSubclassOf<class AAIGatlingGulper> GatlingGulperBlueprint = nullptr;


	//Arrays to save enemies to
	
	TArray<class AAISquidbird*>SpawnedSquidbirdCount;
	TArray<class AAISquidbird*>SpawnedSquidSprintCount;
	TArray<class AAISquidbirdLeaper*>SpawnedSquidLeapCount;
	TArray<class AAIFlameGulper*>SpawnedFlameGulperCount;
	TArray<class AAIGatlingGulper*>SpawnedGatlingGulperCount;


	UPROPERTY(EditAnywhere, Category = "Spawning enemy")
	bool bSquidbird = false;
	
	UPROPERTY(EditAnywhere, Category = "Spawning enemy")
	bool bSquidbirdSprinter = false;

	UPROPERTY(EditAnywhere, Category = "Spawning enemy")
	bool bSquidbirdLeaper = false;

	UPROPERTY(EditAnywhere, Category = "Spawning enemy")
	bool bFlameGulper = false;

	UPROPERTY(EditAnywhere, Category = "Spawning enemy")
	bool bGatlingGulper = false;
	
	UPROPERTY(EditAnywhere, Category= "Spawner Setup")
	UStaticMeshComponent* SpawnerMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Spawner Setup")
	float TimeBetweenSpawn = 2.f;

	UPROPERTY(EditAnywhere, Category = "Spawner Setup")
	int EnemiesSetToSpawn = 1;

	UPROPERTY(EditAnywhere, Category = "Spawner setup")
	int TotalEnemiesToSpawn = 100;

	UFUNCTION()
	void SpawnEnemy();

	UFUNCTION()
	void RespawnEnemy(float DeltaTime);

	UFUNCTION()
	void SpawnSquidBird();

	UFUNCTION()
	void SpawnSquidBirdSprinter();

	UFUNCTION()
	void SpawnSquidBirdLeaper();

	UFUNCTION()
	void SpawnFlameGulper();

	UFUNCTION()
	void SpawnGatlingGulper();

	UFUNCTION()
	void UpdateSquidBirdArray();

	UPROPERTY()
	bool bEnemySetAllowed = true;

private:
	float SpawnTimer = 0.f;
	int SpawnedEnemiesCounter = 0;

	bool bEnemyIsDead = true;

	bool bFirstEnemySpawned = false;

	UFUNCTION()
	int EnemyBoolToInt(bool EnemyBool);

	UFUNCTION()
	void CheckIfMoreThanOneEnemyIsSet();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};


