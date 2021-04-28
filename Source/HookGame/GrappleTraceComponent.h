// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleTraceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOOKGAME_API UGrappleTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleTraceComponent();
	UPROPERTY(EditAnywhere, Instanced, Category="Settings")
	USceneComponent* GrapplePoint;
	UPROPERTY(EditAnywhere, Instanced, Category = "Settings")
	class UBoxComponent* ComponentExtent;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
