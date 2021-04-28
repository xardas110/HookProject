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

int UFallingBridgeComponent::BoolToInt(bool FallMode)
{
	if(FallMode)
	{
		return 1;
	}

	return 0;
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


	
	//float CurrentPitch = GetOwner()->GetActorRotation().Pitch;
	//float CurrentRoll = GetOwner()->GetActorRotation().Roll;
	//float CurrentYaw = GetOwner()->GetActorRotation().Yaw;


	
	FRotator Movement{ TargetPitch,TargetYaw,TargetRoll };

	int CheckNumberOfFallModes
	{
		  BoolToInt(LinearInterpolation)
		+ BoolToInt(InterpolateConstantTo)
		+ BoolToInt(ExponentialInterpolationIn)
		+ BoolToInt(ExponentialInterpolationOut)
		+ BoolToInt(ExponentialInterpolationInOut)
		+ BoolToInt(ConstantStepInterpolation)
	};
	
	if(CheckNumberOfFallModes > 1)
	{
		UE_LOG(LogTemp, Warning,TEXT("%s has MORE than one fallmode active. Please pick just one."), *GetOwner()->GetName())
	}

	else if(CheckNumberOfFallModes == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has NO fallmode active. Please pick one."), *GetOwner()->GetName())
	}

	else
	{
		
		if(LinearInterpolation)
		{

			//Movement.Pitch = FMath::Lerp(CurrentPitch,TargetPitch, FallingSpeed * DeltaTime);
			//Movement.Roll = FMath::Lerp(CurrentRoll, TargetRoll, FallingSpeed * DeltaTime);
			//Movement.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, FallingSpeed * DeltaTime);
				
			Movement = FMath::Lerp(GetOwner()->GetActorRotation(), Movement, FallingSpeed * DeltaTime);
			
			GetOwner()->SetActorRotation(Movement);
		}

		if(InterpolateConstantTo)
		{
			//Movement.Pitch = FMath::FInterpConstantTo(CurrentPitch, TargetPitch, DeltaTime, FallingSpeed);
			//Movement.Roll = FMath::FInterpConstantTo(CurrentRoll, TargetRoll, DeltaTime, FallingSpeed);
			//Movement.Yaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, FallingSpeed);

			GetOwner()->SetActorRotation(Movement);
		}

		if (ExponentialInterpolationIn)
		{
			//Movement.Pitch = FMath::InterpExpoIn(CurrentPitch, TargetPitch, FallingSpeed * DeltaTime);
			//Movement.Roll = FMath::InterpExpoIn(CurrentRoll, TargetRoll, FallingSpeed * DeltaTime);
			//Movement.Yaw = FMath::InterpExpoIn(CurrentYaw, TargetYaw, FallingSpeed * DeltaTime);

			GetOwner()->SetActorRotation(Movement);
		}

		if (ExponentialInterpolationOut)
		{
			//Movement.Pitch = FMath::InterpExpoOut(CurrentPitch, TargetPitch, FallingSpeed * DeltaTime);
			//Movement.Roll = FMath::InterpExpoOut(CurrentRoll, TargetRoll, FallingSpeed * DeltaTime);
			//Movement.Yaw = FMath::InterpExpoOut(CurrentYaw, TargetYaw, FallingSpeed * DeltaTime);

			GetOwner()->SetActorRotation(Movement);
		}

		if (ExponentialInterpolationInOut)
		{
			//Movement.Pitch = FMath::InterpExpoInOut(CurrentPitch, TargetPitch, FallingSpeed * DeltaTime);
			//Movement.Roll = FMath::InterpExpoInOut(CurrentRoll, TargetRoll, FallingSpeed * DeltaTime);
			//Movement.Yaw = FMath::InterpExpoInOut(CurrentYaw, TargetYaw, FallingSpeed * DeltaTime);

			GetOwner()->SetActorRotation(Movement);
		}

		if (ConstantStepInterpolation)
		{
			//Movement.Pitch = FMath::InterpExpoInOut(CurrentPitch, TargetPitch, FallingSpeed * DeltaTime);
			//Movement.Roll = FMath::InterpExpoInOut(CurrentRoll, TargetRoll, FallingSpeed * DeltaTime);
			//Movement.Yaw = FMath::InterpExpoInOut(CurrentYaw, TargetYaw, FallingSpeed * DeltaTime);

			GetOwner()->SetActorRotation(Movement);
		}

	}
	

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

