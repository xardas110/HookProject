#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CameraRotateTrigger.generated.h"

UENUM()
enum RotateFunction
{
	Constant,
	Lerp
};

UCLASS()
class HOOKGAME_API ACameraRotateTrigger : public AActor
{
	GENERATED_BODY()
	
	friend class APlayerBase;
	
public:
	
	ACameraRotateTrigger();
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	TEnumAsByte<RotateFunction> _RotateFunction = RotateFunction::Constant;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bOnBeginOverlapRotation{ false };
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FRotator BeginOverlapRotation{FRotator::ZeroRotator};
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bOnEndOverlapRotation{ false };
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FRotator EndOverlapRotation {FRotator::ZeroRotator};

	UPROPERTY(EditAnywhere, Category = "Settings")
	UBoxComponent* TriggerBox{nullptr};

	bool Reverse{ false };
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	float RotationSpeed{ 1.f };

	void ResetToDefault();
	
protected:
	
	virtual void BeginPlay() override;
	
public:
	
	UFUNCTION()
	void OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
