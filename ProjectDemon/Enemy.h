// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyAIController.h"
#include "EnemyAnimInstance.h"
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
	virtual void LogScreen(FString log, FLinearColor color = FLinearColor::Blue) override;
	virtual void Log(FString log, bool printToScreen = true) override;
	UFUNCTION()
	virtual void ChasePlayer();

	virtual bool GetIsAttackAnimationPlaying();
	
	class UEnemyAnimInstance* EnemyAnimInstance;
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

	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightWeaponSocketInitialPoint = "RightWeaponSocketInitialPoint";
	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightWeaponSocketFinalPoint = "RightWeaponSocketFinalPoint";

	void StartHitbox(float deltaTime, bool bEnableRightPunch = true, bool enableDebug=false);

	void AttackHitbox(FName SocketName, bool bEnableDebug = false);

	void SwordHitbox(FName SocketInitName, FName SocketFinalName);
private:
	bool bEnableCharacterTargetRoataion = false;
	void updateCharacterRotationToTarget(float deltaTime, FVector targetLocation = FVector(0.0), bool enableDebug = false);
public:
	
	UFUNCTION(BlueprintCallable)
	void setEnableCharacterToTargetRotation(bool EnableCharacterTargetRoataion) { bEnableCharacterTargetRoataion = EnableCharacterTargetRoataion; }
	bool getEnableCharacterTargetRoataion() { return bEnableCharacterTargetRoataion; }
	

	/// <summary>
	/// Initiates how the character should react based on who is the sender
	/// </summary>
	/// <param name="sender">The actor who dealt the hit reaction</param>
	/// <returns></returns>
	UFUNCTION()
	virtual float HitReact(AActor* sender);
	UFUNCTION()
	void HitReactEnd(UAnimMontage* animMontage, bool bInterrupted);
	
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
