// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy.h"
#include "BTTask_EnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDEMON_API UBTTask_EnemyBase : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	class AEnemy* Enemy;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
