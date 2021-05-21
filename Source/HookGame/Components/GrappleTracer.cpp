#include "GrappleTracer.h"
#include "../AI/AIBehaviorBase.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

UGrappleTracer::UGrappleTracer()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	ComponentExtent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxExtent"));
	GrapplePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Grapple point"));
	ComponentExtent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ComponentExtent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

