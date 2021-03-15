// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementComponentBase.h"

// Sets default values for this component's properties
UMovementComponentBase::UMovementComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UMovementComponentBase::BeginPlay()
{
	Super::BeginPlay();

	DebugLocation();
	InitialLocation = GetOwner()->GetActorLocation();

	GetOwner()->SetActorLocation(InitialLocation);
	
}


// Called every frame
void UMovementComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(Movable)
	{
		FVector CurrentLocation = GetOwner()->GetActorLocation();

		if (TargetLocation.X > InitialLocation.X)
		{
			CurrentLocation.X += (DeltaTime*MovementSpeed);
		}
		else if (TargetLocation.Y > InitialLocation.Y)
		{
			CurrentLocation.Y += (DeltaTime * MovementSpeed);
		}
		else if (TargetLocation.Z > InitialLocation.Z)
		{
			CurrentLocation.Z += (DeltaTime * MovementSpeed);
		}

		if(CurrentLocation.X >= TargetLocation.X && CurrentLocation.Y >= TargetLocation.Y && CurrentLocation.Z >= TargetLocation.Z)
		{
			StopMovement = true;
		}

		if(!StopMovement)
		{
			GetOwner()->SetActorLocation(CurrentLocation);
		}

		if(StopMovement)
		{
			GetOwner()->SetActorLocation(TargetLocation);
		}

	}

	DebugLocation();

}

void UMovementComponentBase::DebugLocation() {
	FString ObjectName = GetOwner()->GetName();
	FString ObjectLocation = GetOwner()->GetActorTransform().GetLocation().ToString();

	UE_LOG(LogTemp, Warning, TEXT("The location of %s is %s"), *ObjectName, *ObjectLocation);
}