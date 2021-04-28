// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleTracer.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UGrappleTracer::UGrappleTracer()
{
	PrimaryComponentTick.bCanEverTick = true;
	ComponentExtent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxExtent"));
	GrapplePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Grapple point"));
	ComponentExtent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
}

void UGrappleTracer::BeginPlay()
{
	Super::BeginPlay();
	ComponentExtent->SetBoxExtent({ 75.f, 75.f, 150.f });
	AttachToComponent(GetOwner()->GetDefaultAttachComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void UGrappleTracer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GrapplePoint->SetWorldLocation(GetOwner()->GetActorLocation());
	ComponentExtent->SetWorldLocation(GetOwner()->GetActorLocation());
}
