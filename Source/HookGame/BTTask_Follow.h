// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Follow.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API UBTTask_Follow : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Follow();
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
