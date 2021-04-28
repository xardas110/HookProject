// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISquidbird.h"
#include "AISquidbirdLeaper.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAISquidbirdLeaper : public AAISquidbird
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaper Params")
		float LeapDistance = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaper Params")
		FVector LeapForce{3000.f,3000.f,5000.f};

	FVector LeapTarget{ 0.f,0.f,0.f };

	virtual void Attack() override;
	void BeginAttack();
	virtual void Tick(float DeltaSeconds) override;
	void Leap();

	virtual void EndAttack() override;
	

protected:
	FRotator LeapRotation{ 90.f,0.f,0.f };
	FRotator RegularRotation{ 0.f,0.f,0.f };
};
