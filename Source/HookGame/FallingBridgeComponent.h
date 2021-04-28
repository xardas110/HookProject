// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/TriggerVolume.h"
#include "Math/UnrealMathUtility.h"
#include "FallingBridgeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOOKGAME_API UFallingBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFallingBridgeComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	
	//Vectors
	FRotator InitialRotation;

	//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float FallingSpeed = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float TargetPitch = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float TargetYaw = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float TargetRoll = 0.0f;


	//Fall modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall mode - ONLY ONE ACTIVE AT A TIME", meta = (AllowPrivateAccess = "true"))
	bool LinearInterpolation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall mode - ONLY ONE ACTIVE AT A TIME", meta = (AllowPrivateAccess = "true"))
	bool InterpolateConstantTo = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall mode - ONLY ONE ACTIVE AT A TIME", meta = (AllowPrivateAccess = "true"))
	bool ExponentialInterpolationIn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall mode - ONLY ONE ACTIVE AT A TIME", meta = (AllowPrivateAccess = "true"))
	bool ExponentialInterpolationOut = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall mode - ONLY ONE ACTIVE AT A TIME", meta = (AllowPrivateAccess = "true"))
	bool ExponentialInterpolationInOut = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall mode - ONLY ONE ACTIVE AT A TIME", meta = (AllowPrivateAccess = "true"))
	bool ConstantStepInterpolation = false;

	bool HitByPlayer = false;

	//Functions
	UFUNCTION()
	void CheckIfHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	void FallToTargetLocation(float DeltaTime);

	int BoolToInt(bool FallMode);

	class UStaticMeshComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	


		
};
