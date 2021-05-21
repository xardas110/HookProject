#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleActor.generated.h"

UCLASS()
class HOOKGAME_API AGrappleActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AGrappleActor();

protected:

	virtual void BeginPlay() override;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	class UGrappleTraceComponent* TraceComponent;

	virtual void Tick(float DeltaTime) override;
};
