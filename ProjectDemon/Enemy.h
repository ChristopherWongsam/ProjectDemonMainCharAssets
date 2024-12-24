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
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	virtual void ChasePlayer();

	bool GetIsAttackAnimationPlaying();
	

	UPROPERTY(EditAnywhere, Category = Combat)
	float AcceptableAttackRange = 150;
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<UAnimMontage*> HitReactMontageArray;
	int hitReactionCounter = 0;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* LightAttack;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HeavyAttack;
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* RangeAttack;
	UPROPERTY(EditAnywhere, Category = Combat)
	TMap<UAnimMontage*,FString> AttackMontageMap;
	/*Recommend to override. I would not use super.*/
	UFUNCTION()
	virtual float Attack();

	bool bEnableHitBox = false;
	void StartHitbox(float deltaTime, bool bEnableRightPunch = true, bool enableDebug=false);
	void AttackHitbox(FName SocketName);
	/// <summary>
	/// Initiates how the character should react based on who is the sender
	/// </summary>
	/// <param name="sender">The actor who dealt the hit reaction</param>
	/// <returns></returns>
	UFUNCTION()
	virtual float HitReact(AActor* sender);
	TArray< AActor*> actorsHit;
	UFUNCTION()
	void HitReactEnd(UAnimMontage* animMontage, bool bInterrupted);
	//Enables hit box. Will only activate if 
	UFUNCTION(BlueprintCallable)
	void setEnableHitbox(bool enableHitbox);
	UFUNCTION(BlueprintCallable)
	void RestartHitbox();
	UPROPERTY()
	class AEnemyAIController* EnemyController;
	UPROPERTY()
	class ACharacter* MyPlayerCharacter;

	/*Should use enum instead*/
	UPROPERTY(EditAnywhere, Category = Attack)
	bool bEnemyCanAttack = false;

	/*Should use enum instead*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool bEnableMirrorAnimation = false;
};
