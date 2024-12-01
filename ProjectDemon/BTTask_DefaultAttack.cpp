// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DefaultAttack.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	float waitTIme = Enemy->Attack();
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackWaitTime"), waitTIme);
	return EBTNodeResult::Succeeded;
}
