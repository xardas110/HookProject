#include "GrappleTraceComponent.h"
#include "Components/BoxComponent.h"

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
