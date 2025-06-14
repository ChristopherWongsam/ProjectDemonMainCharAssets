// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	Enemy = Cast<AEnemy>(GetPawn());
	if (!Enemy)
	{
		return;
	}
	return;
	if (EnemyBT && Enemy->bEnemyCanAttack)
	{
		RunBehaviorTree(EnemyBT);
		ACharacter* MyPlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		GetBlackboardComponent()->SetValueAsObject(TEXT("AttackTarget"), MyPlayerCharacter);
	}
}
