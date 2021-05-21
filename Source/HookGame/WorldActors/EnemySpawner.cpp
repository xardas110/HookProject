#include "EnemySpawner.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(false);
	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spawner Location"));
	RootComponent = SpawnLocation;
	EnemySpawnTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EnemySpawnTriggerBox"));
	EnemySpawnTriggerBox->SetupAttachment(SpawnLocation);
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.SetTickFunctionEnable(false);
	EnemySpawnTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnBeginOverlap);
	EnemySpawnTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawner::OnEndOverLap);
}

void AEnemySpawner::SpawnEnemy(float DeltaTime)
{
	UWorld* GameWorld = GetWorld();
	ReturnIfNull(GameWorld);

	if (SpawnedEnemyCount.Num() >= EnemiesSetToSpawn)
	{
		SpawnTimer = 0.f;
		return;
	}
	
	SpawnTimer += DeltaTime;
	if (SpawnTimer < TimeBetweenSpawn)
		return;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	
	auto Enemy = GetWorld()->SpawnActor<AAIBehaviorBase>(BehaviorBase,SpawnLocation->GetComponentLocation(),FRotator::ZeroRotator,SpawnParameters);
	if(auto EnemyAi = Cast<AAIBehaviorBase>(Enemy))
	{
		SpawnedEnemyCount.Push(EnemyAi);
	}
}

void AEnemySpawner::CheckForDeadEnemies()
{
	TArray <int> EnemiesToRemove;
	for (int i = 0; i < SpawnedEnemyCount.Num(); i++)
	{
		if(SpawnedEnemyCount[i]->bIsDead)
		{
			EnemiesToRemove.Push(i);
		}
	}

	for(const auto index : EnemiesToRemove)
	{
		SpawnedEnemyCount.RemoveAt(index);
	}
	
}

void AEnemySpawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);

	if (OtherActor->IsA(APlayerBase::StaticClass()))
		PrimaryActorTick.SetTickFunctionEnable(true);
	
}

void AEnemySpawner::OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ReturnIfNull(OtherActor);

	if (OtherActor->IsA(APlayerBase::StaticClass()))
		PrimaryActorTick.SetTickFunctionEnable(false);
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnEnemy(DeltaTime);
	CheckForDeadEnemies();
}

