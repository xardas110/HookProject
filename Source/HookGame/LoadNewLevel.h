// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "LoadNewLevel.generated.h"

UCLASS()
class HOOKGAME_API ALoadNewLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALoadNewLevel();
	friend class APlayerBase;

private:

	UPROPERTY(BlueprintReadWrite, Category = "LevelTransferSettings", meta = (AllowPrivateAccess = "true"))
	bool bHasPlayerOverlapped = false;

	UPROPERTY(EditAnywhere, Category = "LevelTransferSettings")
		UBoxComponent* LevelTransferBox = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
