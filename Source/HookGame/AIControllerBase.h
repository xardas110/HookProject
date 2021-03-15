// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;

	void PatrolFinished();

	bool bPatrolFinished = false;

	

protected:
	AAIControllerBase();
	virtual void BeginPlay() override;

	class UAIParamaterComponent* OwnerParameters{ nullptr };
private:
	UPROPERTY(EditAnywhere)
		class UBehaviorTree* AIBehavior;

};
