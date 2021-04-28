// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "PlayerBase.h"
#include "PowerUpActor.generated.h"

UCLASS()
class HOOKGAME_API APowerUpActor : public AActor
{
	GENERATED_BODY()
	friend class APlayerBase;
	
public:	
	// Sets default values for this actor's properties
	APowerUpActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	UStaticMeshComponent* PowerUpMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	UCapsuleComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float AddedPowerValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up Mode - Try to enable only one at a time", meta = (AllowPrivateAccess = "true"))
	bool HealthPowerUp;

	UPROPERTY()
	FVector InitialLocation = {};

	UPROPERTY()
	FRotator InitialRotation = {};


	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//UPROPERTY()
	//FVector ActorScale;

	//UPROPERTY()
	//FVector StartScale;

	UPROPERTY()
	bool bPlayerHasOverlapped;

	UPROPERTY()
	bool bPlayerHasOverlappedOnce;

	UPROPERTY(EditAnywhere, Category = "Setup")
	bool bInteractable = false;

	UPROPERTY()
	bool ActorIsHidden;

	UPROPERTY(EditAnywhere, Category = "Setup")
	bool ActorRespawnable;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float TimeUntilRespawn = 10.f;

	UPROPERTY()
	float RespawnTimer;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
