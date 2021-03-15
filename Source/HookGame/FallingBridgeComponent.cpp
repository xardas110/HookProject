// Fill out your copyright notice in the Description page of Project Settings.


#include "FallingBridgeComponent.h"
#include "Math/UnrealMathUtility.h"
#include "HookProjectile.h"

// Sets default values for this component's properties
UFallingBridgeComponent::UFallingBridgeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}



// Called when the game starts
void UFallingBridgeComponent::BeginPlay()
{
	Super::BeginPlay();

	InitialRotation = GetOwner()->GetActorRotation();
	
	GetOwner()->OnActorHit.AddDynamic(this, &UFallingBridgeComponent::CheckIfHit);

}


void UFallingBridgeComponent::CheckIfHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor->IsA(AHookProjectile::StaticClass())) 
	{
		HitByPlayer = true;
	}

}


void UFallingBridgeComponent::FallToTargetLocation(float DeltaTime)
{

	float CurrentPitch = GetOwner()->GetActorRotation().Pitch;
	float CurrentRoll = GetOwner()->GetActorRotation().Roll;
	float CurrentYaw = GetOwner()->GetActorRotation().Yaw;

	FRotator FallingBridge{ TargetPitch,TargetYaw,TargetRoll };

	FallingBridge.Pitch = FMath::Lerp(CurrentPitch,TargetPitch, FallingSpeed*DeltaTime);
	FallingBridge.Roll = FMath::Lerp(CurrentRoll, TargetRoll, FallingSpeed * DeltaTime);
	FallingBridge.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, FallingSpeed * DeltaTime);

	GetOwner()->SetActorRotation(FallingBridge);
}


// Called every frame
void UFallingBridgeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HitByPlayer)
	{
		FallToTargetLocation(DeltaTime);

	}

}

