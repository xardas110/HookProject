#include "GrappleActor.h"
#include "../Components/GrappleTraceComponent.h"
#include "Components/BoxComponent.h"

AGrappleActor::AGrappleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grapple Mesh new"));
	TraceComponent = CreateDefaultSubobject<UGrappleTraceComponent>(TEXT("Grapple Trace component new"));

	StaticMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	TraceComponent->GrapplePoint->SetupAttachment(RootComponent);
	TraceComponent->ComponentExtent->SetupAttachment(RootComponent);
}

void AGrappleActor::BeginPlay()
{
	Super::BeginPlay();	
}

void AGrappleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

