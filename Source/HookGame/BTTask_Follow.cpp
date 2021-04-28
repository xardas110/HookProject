// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Follow.h"
#include "AIController.h"
#include "AIBehaviorBase.h"


UBTTask_Follow::UBTTask_Follow()
{
	NodeName = TEXT("Follow");
}

EBTNodeResult::Type UBTTask_Follow::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(OwnerComp.GetAIOwner()==nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't Get AI owner!"));
		return EBTNodeResult::Failed;
	}
	AAIBehaviorBase* Character = Cast<AAIBehaviorBase>(OwnerComp.GetAIOwner()->GetPawn());
	if(Character==nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't Get Character!"));
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
