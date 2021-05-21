#include "Checkpoint.h"
#include "HookGame/Helpers/Macros.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;
	CheckPointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointBox"));
	bCheckpointReached = false;
}

void ACheckpoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);
	
	if (OtherActor->IsA(APlayerBase::StaticClass())&&!bCheckpointReached)
	{
		bCheckpointReached = true;
		
		const auto PlayerReference = Cast<APlayerBase>(OtherActor);
		ReturnIfNull(PlayerReference);
		
		PlayerReference->SaveLatestCheckpoint();
		PlayerReference->bDisplayCheckWidget = true;
	}
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	CheckPointBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBeginOverlap);
}



