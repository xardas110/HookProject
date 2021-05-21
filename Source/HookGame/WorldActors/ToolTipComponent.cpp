#include "ToolTipComponent.h"
#include "../Character/PlayerBase.h"
#include "HookGame/Helpers/Macros.h"

AToolTipComponent::AToolTipComponent()
{
	PrimaryActorTick.bCanEverTick = false;

	ToolTipBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ToolTipBox"));
	RootComponent = ToolTipBox;
	
	bHasPlayerOverlapped = false;	
}

void AToolTipComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);

	if(OtherActor->IsA(APlayerBase::StaticClass())&&!bHasPlayerOverlapped)
	{
		const auto PlayerReference = Cast<APlayerBase>(OtherActor);
		ReturnIfNull(PlayerReference);
		if(PlayerReference->bNPCFollowPlayer)
		{
			PlayerReference->ToolTipTrigger(ToolTipDuration, ToolTipText);
			bHasPlayerOverlapped = true;			
			Destroy();
		}
	}
}

void AToolTipComponent::BeginPlay()
{
	Super::BeginPlay();

	ToolTipBox->OnComponentBeginOverlap.AddDynamic(this, &AToolTipComponent::OnBeginOverlap);
}

void AToolTipComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}