// Fill out your copyright notice in the Description page of Project Settings.


#include "AIParamaterComponent.h"
#include "Engine/World.h"
#include "Math/Vector.h"

// Sets default values for this component's properties
UAIParamaterComponent::UAIParamaterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAIParamaterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAIParamaterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector MyPosition = GetOwner()->GetActorLocation();
	FVector PlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	float PlayerLastSeen = 0.f;
	if (FVector::Dist(MyPosition, PlayerPosition) < DetectionRange) {
		bPlayerInRange = true;
		bReturnToStart = false;
		PlayerLastSeen = GetWorld()->GetTimeSeconds();
	}
	else {
		bPlayerInRange=false;
	}

	if (GetWorld()->GetTimeSeconds() > PlayerLastSeen + ResetTimer) {
		bReturnToStart = true;
	}
	// ...
}

