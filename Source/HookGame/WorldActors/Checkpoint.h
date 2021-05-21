#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "../Character/PlayerBase.h"
#include "Checkpoint.generated.h"

UCLASS()
class HOOKGAME_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	

	ACheckpoint();

	UPROPERTY(EditAnywhere, Category = "CheckPointSettings")
	bool bCheckpointReached = false;

	UPROPERTY(EditAnywhere, Category = "CheckPointSettings")
	UBoxComponent* CheckPointBox = nullptr;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
