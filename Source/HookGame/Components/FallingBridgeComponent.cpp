#include "FallingBridgeComponent.h"
#include "Math/UnrealMathUtility.h"
#include "../Character/Actors/HookProjectile.h"

UFallingBridgeComponent::UFallingBridgeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFallingBridgeComponent::BeginPlay()
{
	Super::BeginPlay();

	InitialRotation = GetOwner()->GetActorRotation();
	GetOwner()->OnActorHit.AddDynamic(this, &UFallingBridgeComponent::CheckIfHit);
}

void UFallingBridgeComponent::CheckIfHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(AHookProjectile::StaticClass())) 
		HitByPlayer = true;
}

void UFallingBridgeComponent::FallToTargetLocation(float DeltaTime)
{
	FRotator Movement{ TargetPitch,TargetYaw,TargetRoll };

	switch (InterPFunction)
	{
	case 1:
		Movement = FMath::Lerp(GetOwner()->GetActorRotation(), Movement, FallingSpeed * DeltaTime);			
		GetOwner()->SetActorRotation(Movement);
		break;
	case 2:
		Movement = FMath::RInterpConstantTo(GetOwner()->GetActorRotation(), Movement, DeltaTime, FallingSpeed);
		GetOwner()->SetActorRotation(Movement);
		break;
	//case 3:
	//	Movement = FMath::RInterpTo(GetOwner()->GetActorRotation(), Movement, DeltaTime, FallingSpeed);
	//	GetOwner()->SetActorRotation(Movement);
	//	break;
	default:
		break;
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

