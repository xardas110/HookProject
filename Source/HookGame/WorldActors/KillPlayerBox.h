#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "KillPlayerBox.generated.h"

UCLASS()
class HOOKGAME_API AKillPlayerBox : public AActor
{
	GENERATED_BODY()
	
public:	

	AKillPlayerBox();
	friend class APlayerBase;

private:

	UPROPERTY(EditAnywhere, Category = "KillBoxSettings")
	bool bHasPlayerOverlapped = false;

	UPROPERTY(EditAnywhere, Category = "KillBoxSettings")
	float DamageValue = 1000.f; //Set high to kill the player instantly. Can be adjusted if needed for different purposes.

	UPROPERTY(EditAnywhere, Category = "KillBoxSettings")
	UBoxComponent* DamageBox = nullptr;
	
protected:

	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
