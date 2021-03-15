// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "AIBehaviorBase.h"

UBTTask_Attack::UBTTask_Attack() {
	
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Couldn't Get AI owner!"));
		return EBTNodeResult::Failed;
		
	}
	AAIBehaviorBase* Character = Cast<AAIBehaviorBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (Character == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Couldn't Get Character!"));
		return EBTNodeResult::Failed;
	}

	Character->Attack();

	return EBTNodeResult::Succeeded;
}
