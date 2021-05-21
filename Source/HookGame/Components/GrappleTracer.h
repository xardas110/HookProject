#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GrappleTracer.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOOKGAME_API UGrappleTracer : public USceneComponent
{
	GENERATED_BODY()

public:	
	UGrappleTracer();
	
	UPROPERTY(EditAnywhere,Category = "Settings")
	USceneComponent* GrapplePoint;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	class UBoxComponent* ComponentExtent;
};
