#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "../Components/GrappleTracer.h"
#include "PowerUpActor.generated.h"

UCLASS()
class HOOKGAME_API APowerUpActor : public AActor
{
	GENERATED_BODY()
	friend class APlayerBase;
	
public:	

	APowerUpActor();

	UPROPERTY(EditAnywhere, Category = Tracer)
	UGrappleTracer* GrappleTracer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	UStaticMeshComponent* PowerUpMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	UCapsuleComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float AddedPowerValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up Mode - Try to enable only one at a time", meta = (AllowPrivateAccess = "true"))
	bool HealthPowerUp;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Setup")
	bool bInteractable = false;

protected:

	virtual void BeginPlay() override;

};
