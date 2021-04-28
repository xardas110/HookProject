#include "GrappleTraceComponent.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UGrappleTraceComponent::UGrappleTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ComponentExtent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxExtent"));
	GrapplePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Grapple point"));
	ComponentExtent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
}

void UGrappleTraceComponent::BeginPlay()
{
	Super::BeginPlay();
}
