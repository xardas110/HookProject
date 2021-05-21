#include "ExperienceComponent.h"
#include "../AI/AIBehaviorBase.h"
#include "Kismet/GameplayStatics.h"

UExperienceComponent::UExperienceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ConstructorHelpers::FClassFinder<AXpBall> Obj(TEXT("/Game/Blueprints/WorldActors/BP_XpBall"));
	if (Obj.Succeeded())
		XpBallClass = Obj.Class;
}
void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UExperienceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (const auto AIBase = Cast<AAIBehaviorBase>(GetOwner()))
	{
		if (AIBase->HealthTotal <= 0.f)
		{
			SpawnXpOnDeath(AIBase->GetActorLocation());
			DestroyComponent();
		}
	}		
}

void UExperienceComponent::SpawnXpOnDeath(const FVector& ActorLocation)
{
	for (int32 i = 0; i < NrXpBalls; i++)
	{
		auto NewLocation = ActorLocation;
		const FVector HalfSize = RandomBounds * 0.5f;
		NewLocation.X += FMath::FRandRange(-HalfSize.X, HalfSize.X);
		NewLocation.Y += FMath::FRandRange(-HalfSize.Y, HalfSize.Y);
		NewLocation.Z += FMath::FRandRange(-HalfSize.Z, HalfSize.Z);
		
		GetWorld()->SpawnActor<AXpBall>(XpBallClass, NewLocation, FRotator::ZeroRotator);
	}
}
