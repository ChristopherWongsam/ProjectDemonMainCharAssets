// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyBase.h"
#include "AIController.h"
#include "Enemy.h"


EBTNodeResult::Type UBTTask_EnemyBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (OwnerComp.GetAIOwner())
	{
		Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	}
	return EBTNodeResult::Type();
}
