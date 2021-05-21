#include "AudioTrigger.h"

AAudioTrigger::AAudioTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	AudioTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EnemySpawnTriggerBox"));
	RootComponent = AudioTriggerBox;
	SoundEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	
	bPlayerHasOverlapped = false;
}

void AAudioTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);
	ReturnIfNull(SoundEffect);
	if (OtherActor->IsA(APlayerBase::StaticClass())&&!bPlayerHasOverlapped)
	{
		bPlayerHasOverlapped = true;
		SoundEffect->Play();
	}
}


void AAudioTrigger::BeginPlay()
{
	Super::BeginPlay();
	AudioTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAudioTrigger::OnBeginOverlap);
}

void AAudioTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

