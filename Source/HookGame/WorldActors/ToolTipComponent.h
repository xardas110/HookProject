#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ToolTipComponent.generated.h"

UCLASS()
class HOOKGAME_API AToolTipComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	AToolTipComponent();

	friend class APlayerBase;

	UPROPERTY(EditAnywhere, Category = "ToolTipSettings")
	bool bHasPlayerOverlapped = false;

	UPROPERTY(EditAnywhere, Category = "ToolTipSettings")
	UBoxComponent* ToolTipBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ToolTipSettings")
	FString ToolTipText = "Remember to set the text in here";

	UPROPERTY(EditAnywhere, Category = "ToolTipSettings")
	float ToolTipDuration = 5.f;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
};