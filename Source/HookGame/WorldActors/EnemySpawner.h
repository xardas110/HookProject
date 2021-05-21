#pragma once

#include "CoreMinimal.h"
#include "../AI/AIBehaviorBase.h"
#include "../Character/PlayerBase.h"
#include "../Helpers/Macros.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class HOOKGAME_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Spawner Settings")
	TSubclassOf<AAIBehaviorBase> BehaviorBase;

	//Array to save enemies to
	TArray<AAIBehaviorBase*>SpawnedEnemyCount;

	UPROPERTY(EditAnywhere)
	USceneComponent* SpawnLocation {nullptr};

	UPROPERTY(EditAnywhere)
	UBoxComponent* EnemySpawnTriggerBox {nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Spawner Setup")
	float TimeBetweenSpawn = 2.f;

	UPROPERTY(EditAnywhere, Category = "Spawner Setup")
	int EnemiesSetToSpawn = 1;

	UPROPERTY(EditAnywhere, Category = "Spawner setup")
	int TotalEnemiesToSpawn = 100;

	UFUNCTION()
	void SpawnEnemy(float DeltaTime);

	void CheckForDeadEnemies();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	float SpawnTimer = 0.f;
	
public:	

	virtual void Tick(float DeltaTime) override;
	
};


