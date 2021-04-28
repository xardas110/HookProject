// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleActor.generated.h"

UCLASS()
class HOOKGAME_API AGrappleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrappleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	class UGrappleTraceComponent* TraceComponent;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
