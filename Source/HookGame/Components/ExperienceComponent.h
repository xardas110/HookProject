#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../WorldActors/XpBall.h"
#include "ExperienceComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOOKGAME_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 NrXpBalls = 10;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector RandomBounds {1000.f, 1000.f, 1000.f};
public:	
	// Sets default values for this component's properties
	UExperienceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, Category = "Mesh spawn")
	TSubclassOf<AXpBall> XpBallClass;

	void SpawnXpOnDeath(const FVector& Location);
};