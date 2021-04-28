// Fill out your copyright notice in the Description page of Project Settings.

#include "GrappleActor.h"
#include "GrappleTraceComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AGrappleActor::AGrappleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grapple Mesh new"));
	TraceComponent = CreateDefaultSubobject<UGrappleTraceComponent>(TEXT("Grapple Trace component new"));

	UE_LOG(LogTemp, Warning, TEXT("Grapple actor created"));
	StaticMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	TraceComponent->GrapplePoint->SetupAttachment(RootComponent);
	TraceComponent->ComponentExtent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGrappleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrappleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

