// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDEMON_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	UMaterialInterface* currMaterial;
	void enableOutline(bool enableOutline);
	bool enemyIsHighlighted();
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void ChasePlayer();

	UPROPERTY(EditAnywhere, Category = Attack)
	float AcceptableAttackRange = 150;
	UPROPERTY(EditAnywhere, Category = Attack)
	TArray<UAnimMontage*> AttackMontageArray;
	UPROPERTY(EditAnywhere, Category = Attack)
	UAnimMontage* LightAttack;
	UPROPERTY(EditAnywhere, Category = Attack)
	UAnimMontage* HeavyAttack;
	UPROPERTY(EditAnywhere, Category = Attack)
	UAnimMontage* RangeAttack;
	/*Recommend to override. I would not use super.*/
	UFUNCTION()
	virtual float Attack();
	UPROPERTY()
	class AEnemyAIController* EnemyController;
	UPROPERTY()
	class ACharacter* MyPlayerCharacter;

	/*Should use enum instead*/
	UPROPERTY(EditAnywhere, Category = Attack)
	bool bEnemyCanAttack = false;
};
