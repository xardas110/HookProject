#pragma once

#include "CoreMinimal.h"
#include "../Character/PlayerBase.h"
#include "../Helpers/Macros.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "AudioTrigger.generated.h"

UCLASS()
class HOOKGAME_API AAudioTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	
	AAudioTrigger();

protected:

	UPROPERTY(EditAnywhere, Category = "Settings")
	UAudioComponent* SoundEffect {nullptr};

	UPROPERTY(EditAnywhere)
	UBoxComponent* AudioTriggerBox {nullptr};
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bPlayerHasOverlapped = false;
	
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
