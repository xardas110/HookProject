#include "LoadNewLevel.h"
#include "HookGame/Helpers/Macros.h"

ALoadNewLevel::ALoadNewLevel()
{
	PrimaryActorTick.bCanEverTick = true;

	LevelTransferBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LevelTransferBox"));

	bHasPlayerOverlapped = false;
}

void ALoadNewLevel::BeginPlay()
{
	Super::BeginPlay();

	LevelTransferBox->OnComponentBeginOverlap.AddDynamic(this, &ALoadNewLevel::OnBeginOverlap);	
}

void ALoadNewLevel::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);

	if(OtherActor->IsA(APlayerBase::StaticClass()))
		bHasPlayerOverlapped = true;	
}

void ALoadNewLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}