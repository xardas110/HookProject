// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CameraRotateTrigger.generated.h"

UCLASS()
class HOOKGAME_API ACameraRotateTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ACameraRotateTrigger();
	friend class APlayerBase;
private:

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
