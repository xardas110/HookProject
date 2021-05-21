#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "LoadNewLevel.generated.h"

UCLASS()
class HOOKGAME_API ALoadNewLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	ALoadNewLevel();
	friend class APlayerBase;

private:

	UPROPERTY(BlueprintReadWrite, Category = "LevelTransferSettings", meta = (AllowPrivateAccess = "true"))
	bool bHasPlayerOverlapped = false;

	UPROPERTY(EditAnywhere, Category = "LevelTransferSettings")
		UBoxComponent* LevelTransferBox = nullptr;

protected:

	virtual void BeginPlay() override;

public:

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Tick(float DeltaTime) override;

};
