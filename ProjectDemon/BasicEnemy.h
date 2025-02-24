// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "DemonCharacter.h"
#include "BasicEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDEMON_API ABasicEnemy : public AEnemy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = PlayerSearch)
	FName headForwardEndSocketName = "headForwardEndSocket";
	UPROPERTY(EditAnywhere, Category = PlayerSearch)
	FName headForwardStartSocket = "headForwardStartSocket";

	

	virtual void Tick(float DeltaTime) override;

	bool bPlayerFound = false;
	void UpdateHeadFindPlayer(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeMontage;

	UFUNCTION()
	void OnDodgeEnd(UAnimMontage* Montage, bool interrupted);
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	float EnemyAttackRange = 250.0;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	float EnemyJumpAttackRange = 500.0;
	void UpdateMoveToPlayer(float DeltaTime);

	/*Recommend to override. I would not use super.*/
	virtual float Attack() override;

	FVector EnemyMoveToLocation;
	FVector GetStrafeLocation();

	bool bEnablePlayerRangeDecsion = true;
	UFUNCTION()
	void EnablePlayerRangeDecision();

	virtual bool GetIsAttackAnimationPlaying() override;

	bool GetIsDodgeAnimationPlaying();

	virtual float HitReact(AActor* sender) override;
};
