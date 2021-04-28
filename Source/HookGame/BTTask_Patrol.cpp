// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"
#include "AIController.h"
#include "AIControllerBase.h"

UBTTask_Patrol::UBTTask_Patrol() {
	NodeName = TEXT("MoveToPatrol");

	
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Couldn't get AI owner"));
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Succeeded;
}
