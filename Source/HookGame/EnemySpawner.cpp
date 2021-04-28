// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawner Mesh"));
	SpawnerMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	CheckIfMoreThanOneEnemyIsSet();


	
}

void AEnemySpawner::SpawnEnemy()
{
	if (bEnemySetAllowed)
	{
		if (bSquidbird)
		{
			SpawnSquidBird();
		}

		if (bSquidbirdSprinter)
		{
			SpawnSquidBirdSprinter();
		}

		if (bSquidbirdLeaper)
		{
			SpawnSquidBirdLeaper();
		}

		if (bFlameGulper)
		{
			SpawnFlameGulper();
		}

		if (bGatlingGulper)
		{
			SpawnGatlingGulper();
		}

	}
}

void AEnemySpawner::RespawnEnemy(float DeltaTime)
{

	SpawnTimer += DeltaTime;

	if (bEnemySetAllowed)
	{
		if (bSquidbird)
		{
			SpawnSquidBird();
		}

		if (bSquidbirdSprinter)
		{
			SpawnSquidBirdSprinter();
		}

		if (bSquidbirdLeaper)
		{
			SpawnSquidBirdLeaper();
		}

		if (bFlameGulper)
		{
			SpawnFlameGulper();
		}

		if (bGatlingGulper)
		{
			SpawnGatlingGulper();
		}

	}
	
}

void AEnemySpawner::SpawnSquidBird()
{
	UWorld* GameWorld = GetWorld();
	if (!GameWorld && !SquidbirdBlueprint)
		return;

	TArray <int> EnemiesToRemove;
	for (int ix = 0; ix < SpawnedSquidbirdCount.Num(); ix++)
	{
		if (!SpawnedSquidbirdCount[ix] || SpawnedSquidbirdCount[ix]->IsPendingKill() || SpawnedSquidbirdCount[ix]->bIsDead)
		{
			EnemiesToRemove.Push(ix);
			UE_LOG(LogTemp, Warning, TEXT("ForLoop1: EnmiesToRemovePushed"))
		}
	}

	for (auto index : EnemiesToRemove)
	{
		SpawnedSquidbirdCount.RemoveAt(index);
		UE_LOG(LogTemp, Warning, TEXT("ForLoop2: Index entered"))
		bEnemyIsDead = true;
		SpawnTimer = 0.f;
	}
	
	if(SpawnedSquidbirdCount.Num() <= EnemiesSetToSpawn && !bFirstEnemySpawned)
	{
		FVector SpawnLocation{ 0.f,30.f,200.f };
		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				AAISquidbird* SpawnedActor = GameWorld->SpawnActor<AAISquidbird>(SquidbirdBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedSquidbirdCount.Push(SpawnedActor);
					bFirstEnemySpawned = true;
					UE_LOG(LogTemp, Warning, TEXT("First EnemySpawned is true"))
					bEnemyIsDead = false;
				}

				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: Squidbird"));
			}
		}	
	}

	
	if (SpawnTimer > TimeBetweenSpawn && bFirstEnemySpawned && SpawnedSquidbirdCount.Num() <= EnemiesSetToSpawn && bEnemyIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Went into respawner"))

		SpawnTimer = 0.f;
		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				AAISquidbird* SpawnedActor = GameWorld->SpawnActor<AAISquidbird>(SquidbirdBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if(SpawnedActor)
				{
					SpawnedSquidbirdCount.Push(SpawnedActor);
					UE_LOG(LogTemp, Warning, TEXT("Respawn enemy spawned"))
					bEnemyIsDead = false;
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: Squidbird"));
			}
		}


	}
}

void AEnemySpawner::SpawnSquidBirdSprinter()
{

	UWorld* GameWorld = GetWorld();
	if (!GameWorld && !SquidbirdSprinterBlueprint)
		return;

	TArray <int> EnemiesToRemove;
	for (int ix = 0; ix < SpawnedSquidSprintCount.Num(); ix++)
	{
		if (!SpawnedSquidSprintCount[ix] || SpawnedSquidSprintCount[ix]->IsPendingKill() || SpawnedSquidSprintCount[ix]->bIsDead)
		{
			EnemiesToRemove.Push(ix);
			UE_LOG(LogTemp, Warning, TEXT("ForLoop1: EnmiesToRemovePushed"))
		}
	}

	for (auto index : EnemiesToRemove)
	{
		SpawnedSquidSprintCount.RemoveAt(index);
		UE_LOG(LogTemp, Warning, TEXT("ForLoop2: Index entered"))
		bEnemyIsDead = true;
		SpawnTimer = 0.f;
	}


	if (SpawnedSquidSprintCount.Num() <= EnemiesSetToSpawn && !bFirstEnemySpawned){

		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				AAISquidbird* SpawnedActor = GameWorld->SpawnActor<AAISquidbird>(SquidbirdSprinterBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedSquidSprintCount.Add(SpawnedActor);
					bFirstEnemySpawned = true;
					bEnemyIsDead = false;
					SpawnedEnemiesCounter++;

					UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: SquidbirdSprinter"));
				}

			}
		}
	}
	
	
	if (SpawnTimer > TimeBetweenSpawn && bFirstEnemySpawned && SpawnedSquidSprintCount.Num() <= EnemiesSetToSpawn && bEnemyIsDead)
	{
		SpawnTimer = 0.f;

		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{

				AAISquidbird* SpawnedActor = GameWorld->SpawnActor<AAISquidbird>(SquidbirdSprinterBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedSquidSprintCount.Add(SpawnedActor);
					bEnemyIsDead = false;
					bEnemyIsDead = false;
					SpawnedEnemiesCounter++;
					
					UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: SquidbirdSprinter"));
				}
				
			}
		}

	}
}

void AEnemySpawner::SpawnSquidBirdLeaper()
{
	UWorld* GameWorld = GetWorld();
	if (!GameWorld && !SquidbirddLeaperBlueprint)
		return;

	TArray <int> EnemiesToRemove;
	for (int ix = 0; ix < SpawnedSquidLeapCount.Num(); ix++)
	{
		if (!SpawnedSquidLeapCount[ix] || SpawnedSquidLeapCount[ix]->IsPendingKill() || SpawnedSquidLeapCount[ix]->bIsDead)
		{
			EnemiesToRemove.Push(ix);
			UE_LOG(LogTemp, Warning, TEXT("ForLoop1: EnmiesToRemovePushed"))
		}
	}

	for (auto index : EnemiesToRemove)
	{
		SpawnedSquidLeapCount.RemoveAt(index);
		UE_LOG(LogTemp, Warning, TEXT("ForLoop2: Index entered"))
		bEnemyIsDead = true;
		SpawnTimer = 0.f;
	}

	if(SpawnedSquidLeapCount.Num() <= EnemiesSetToSpawn && !bFirstEnemySpawned)
	{
		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				SpawnedEnemiesCounter++;

				AAISquidbirdLeaper* SpawnedActor = GameWorld->SpawnActor<AAISquidbirdLeaper>(SquidbirddLeaperBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedSquidLeapCount.Add(SpawnedActor);
					bFirstEnemySpawned = true;
					bEnemyIsDead = false;
				}

				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: SquidbirdLeaper"));
			}
		}

	}
	
	if (SpawnTimer > TimeBetweenSpawn && bFirstEnemySpawned && SpawnedSquidLeapCount.Num() <= EnemiesSetToSpawn && bEnemyIsDead)
	{

		SpawnTimer = 0.f;

		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				SpawnedEnemiesCounter++;

				AAISquidbirdLeaper* SpawnedActor = GameWorld->SpawnActor<AAISquidbirdLeaper>(SquidbirddLeaperBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedSquidLeapCount.Add(SpawnedActor);
					bEnemyIsDead = false;
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: SquidbirdLeaper"));
			}
		}


	}
}

void AEnemySpawner::SpawnFlameGulper()
{
	UWorld* GameWorld = GetWorld();
	if (!GameWorld && !FlamegulperBlueprint)
		return;

	TArray <int> EnemiesToRemove;
	for (int ix = 0; ix < SpawnedFlameGulperCount.Num(); ix++)
	{
		if (!SpawnedFlameGulperCount[ix] || SpawnedFlameGulperCount[ix]->IsPendingKill() || SpawnedFlameGulperCount[ix]->bIsDead)
		{
			EnemiesToRemove.Push(ix);
			UE_LOG(LogTemp, Warning, TEXT("ForLoop1: EnmiesToRemovePushed"))
		}
	}

	for (auto index : EnemiesToRemove)
	{
		SpawnedFlameGulperCount.RemoveAt(index);
		UE_LOG(LogTemp, Warning, TEXT("ForLoop2: Index entered"))
		bEnemyIsDead = true;
		SpawnTimer = 0.f;
	}

	
	if (SpawnedFlameGulperCount.Num() <= EnemiesSetToSpawn && !bFirstEnemySpawned)
	{

		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				SpawnedEnemiesCounter++;

				AAIFlameGulper* SpawnedActor = GameWorld->SpawnActor<AAIFlameGulper>(FlamegulperBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedFlameGulperCount.Add(SpawnedActor);
					bFirstEnemySpawned = true;
					bEnemyIsDead = false;
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: FlameGulper"));
			}
		}
	

	}


	if (SpawnTimer > TimeBetweenSpawn && bFirstEnemySpawned && SpawnedFlameGulperCount.Num() <= EnemiesSetToSpawn && bEnemyIsDead)
	{

		SpawnTimer = 0.f;

		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				SpawnedEnemiesCounter++;

				AAIFlameGulper* SpawnedActor = GameWorld->SpawnActor<AAIFlameGulper>(FlamegulperBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedFlameGulperCount.Add(SpawnedActor);
					bEnemyIsDead = false;
				}

				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: FlameGulper"));
			}
		}
	}
}

void AEnemySpawner::SpawnGatlingGulper()
{

	UWorld* GameWorld = GetWorld();
	if (!GameWorld && !GatlingGulperBlueprint)
		return;

	TArray <int> EnemiesToRemove;
	for (int ix = 0; ix < SpawnedGatlingGulperCount.Num(); ix++)
	{
		if (!SpawnedGatlingGulperCount[ix] || SpawnedGatlingGulperCount[ix]->IsPendingKill() || SpawnedGatlingGulperCount[ix]->bIsDead)
		{
			EnemiesToRemove.Push(ix);
			UE_LOG(LogTemp, Warning, TEXT("ForLoop1: EnmiesToRemovePushed"))
		}
	}

	for (auto index : EnemiesToRemove)
	{
		SpawnedGatlingGulperCount.RemoveAt(index);
		UE_LOG(LogTemp, Warning, TEXT("ForLoop2: Index entered"))
		bEnemyIsDead = true;
		SpawnTimer = 0.f;
	}

	if (SpawnedFlameGulperCount.Num() <= EnemiesSetToSpawn && !bFirstEnemySpawned)
	{
		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				SpawnedEnemiesCounter++;

				AAIGatlingGulper* SpawnedActor = GameWorld->SpawnActor<AAIGatlingGulper>(GatlingGulperBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedGatlingGulperCount.Add(SpawnedActor);
					bFirstEnemySpawned = true;
					bEnemyIsDead = false;
				}

				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: GatlingGulper"));
			}
		}

	}
	
	if (SpawnTimer > TimeBetweenSpawn && bFirstEnemySpawned && SpawnedGatlingGulperCount.Num() <= EnemiesSetToSpawn && bEnemyIsDead)
	{

		SpawnTimer = 0.f;

		FVector SpawnLocation{ 0.f,30.f,200.f };

		if (TotalEnemiesToSpawn > SpawnedEnemiesCounter)
		{

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			for (int ix = 0; ix < EnemiesSetToSpawn; ix++)
			{
				SpawnedEnemiesCounter++;

				AAIGatlingGulper* SpawnedActor = GameWorld->SpawnActor<AAIGatlingGulper>(GatlingGulperBlueprint, GetActorLocation() + SpawnLocation, GetActorRotation(), SpawnParameters);

				if (SpawnedActor)
				{
					SpawnedGatlingGulperCount.Add(SpawnedActor);
					bEnemyIsDead = false;
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Spawned an enemytype: GatlingGulper"));
			}
		}
	}

}

void AEnemySpawner::UpdateSquidBirdArray()
{

	TArray <int> EnemiesToRemove;
	for (int ix = 0; ix < SpawnedSquidbirdCount.Num(); ix++)
	{
		if (!SpawnedSquidbirdCount[ix] || SpawnedSquidbirdCount[ix]->IsPendingKill() || SpawnedSquidbirdCount[ix]->bIsDead)
		{
			EnemiesToRemove.Push(ix);
			UE_LOG(LogTemp, Warning, TEXT("ForLoop1: EnmiesToRemovePushed"))
		}
	}

	for (auto index : EnemiesToRemove)
	{
		SpawnedSquidbirdCount.RemoveAt(index);
		UE_LOG(LogTemp, Warning, TEXT("ForLoop2: Index entered"))
	}
	
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RespawnEnemy(DeltaTime);

}

int AEnemySpawner::EnemyBoolToInt(bool EnemyBool)
{

	if (EnemyBool)
	{
		return 1;
	}

	return 0;
	
}



void AEnemySpawner::CheckIfMoreThanOneEnemyIsSet()
{
	//int CheckNumberOfEnemiesSet
	//{
	//	  EnemyBoolToInt(bSquidbird)
	//	+ EnemyBoolToInt(bSquidbirdSprinter)
	//	+ EnemyBoolToInt(bSquidbirdLeaper)
	//	+ EnemyBoolToInt(bFlameGulper)
	//	+ EnemyBoolToInt(bGatlingGulper)
	//};

	//if (CheckNumberOfEnemiesSet > 1)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s has MORE than one enemy set to spawn. Please pick just one."), *GetOwner()->GetName())
	//	bEnemySetAllowed = false;
	//}

	//else if (CheckNumberOfEnemiesSet == 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s has NO enemy set to spawn. Please pick one."), *GetOwner()->GetName())
	//	bEnemySetAllowed = false;
	//}

}