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
	UPROPERTY(EditAnywhere, Category = PlayerSearch)
	float CharacterDetectionShpereRadius = 2500.0;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	bool bPlayerFound = false;
	void UpdateHeadFindPlayer(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeForwardMontage;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeFowLeftMontage;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeFowRightMontage;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeLeftMontage;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	UAnimMontage* DodgeRightMontage;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	TMap<UAnimMontage*, UAnimMontage*> DodgMontageChainMap;


	UFUNCTION()
	void OnDodgeEnd(UAnimMontage* Montage, bool interrupted);
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	float EnemyAttackRange = 200.0;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	float EnemyRadiusRange = 600;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	float EnemyJumpAttackRange = 500.0;
	UPROPERTY(EditAnywhere, Category = EnemyAiMovement)
	float DodgeMovementScale = 1.0;
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

	UFUNCTION()
	void OnAttackEnd(UAnimMontage* Montage, bool interrupted);

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* CounterMontage;
};
