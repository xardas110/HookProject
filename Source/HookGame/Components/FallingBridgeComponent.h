#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FallingBridgeComponent.generated.h"

UENUM()
enum InterpFunction
{
	LinearInterpolation,
	InterpolateConstantTo,
	InterpolateTo
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<InterpFunction> InterPFunction = InterpFunction::InterpolateConstantTo;

	bool HitByPlayer = false;

	//Functions
	UFUNCTION()
	void CheckIfHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	void FallToTargetLocation(float DeltaTime);

	class UStaticMeshComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	


		
};
