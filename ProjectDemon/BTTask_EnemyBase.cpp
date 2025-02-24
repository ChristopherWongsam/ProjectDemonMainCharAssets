// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyBase.h"
#include "AIController.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>


EBTNodeResult::Type UBTTask_EnemyBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (OwnerComp.GetAIOwner())
	{
		Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	}
	MyPlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	return EBTNodeResult::Type();
}
