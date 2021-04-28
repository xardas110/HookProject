
#include "ExperienceComponent.h"
#include "Components/SphereComponent.h"
#include "AIBehaviorBase.h"
#include "Kismet/GameplayStatics.h"

bool inline PrintDime(const char* str)
{
	const FString FStr(str);
	UE_LOG(LogTemp, Warning, TEXT("%s returned nullptr"), *FStr);
	return false;
}

#define ReturnIfNull(name) if (!name) if (!PrintDime(#name)) return

UExperienceComponent::UExperienceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ConstructorHelpers::FClassFinder<AXpBall> Obj(TEXT("/Game/Blueprints/BP_XpBall"));
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
	
	ReturnIfNull(GetOwner());
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
	ReturnIfNull(GetOwner());
	for (int32 i = 0; i < NrXpBalls; i++)
	{
		auto NewLocation = ActorLocation;
		NewLocation.Z += 300.f;
		GetWorld()->SpawnActor<AXpBall>(XpBallClass, NewLocation, FRotator::ZeroRotator);
	}
}
