#include "KillPlayerBox.h"
#include "../Character/PlayerBase.h"
#include "HookGame/Helpers/Macros.h"

AKillPlayerBox::AKillPlayerBox()
{
	PrimaryActorTick.bCanEverTick = true;
	
	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));

	bHasPlayerOverlapped = false;
}

// Called when the game starts or when spawned
void AKillPlayerBox::BeginPlay()
{
	Super::BeginPlay();

	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &AKillPlayerBox::OnBeginOverlap);
}

void AKillPlayerBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);

	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player overlapped a kill box."))

		bHasPlayerOverlapped = true;

		const auto PlayerReference = Cast<APlayerBase>(OtherActor);
		ReturnIfNull(PlayerReference);
		
		FDamageEvent DamagableEvent;
		
		const auto PlayerControllerRef = PlayerReference->Controller;
		
		UE_LOG(LogTemp, Warning, TEXT("Player hit a kill box and was dealt damage."))

		PlayerReference->TakeDamage(DamageValue, DamagableEvent, PlayerControllerRef, this);			
	}
}

