// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectDemonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include "DemonCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ADemonCharacter : public AProjectDemonCharacter
{
	GENERATED_BODY()

	


	TArray<AActor*> actorsToIgnore;
	class UMyMainCharacterAnimInstance* MainCharacterAnimInstance;

	
	
	
public:
	/** Left Mouse Action Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftMouseAction;
	/** Left Shift Action Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftShiftAction;
	/** Left CTRL Action Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftCtrltAction;

	UPROPERTY(EditAnywhere, Category = Mantle)
	UAnimMontage* MantleMontage;
	UPROPERTY(EditAnywhere, Category = Mantle)
	float mantleZOffsset = 10;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Used for debugging of drawing the playewr input
	void DrawInput(float DeltaTime);

	
	void Mantle(float DeltaTime);
	void MantleEnd(UAnimMontage* animMontage, bool bInterrupted);

	
	//Free flow setup
	void StartFreeflow(bool enableDebug=false);
	void freeflowEnd(UAnimMontage* animMontage, bool bInterrupted);
	bool bCanGoToNextFreeflow = true;
	//How Far to the distance should be from the target when player reaches target
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Freeflow, meta = (AllowPrivateAccess = "true"))
	float freeflowDisFromTarget = 50.0f;

	UFUNCTION(BlueprintCallable)
	void canGoToNextfreeflow();

	void PlayerAttackEnd(UAnimMontage* animMontage, bool bInterrupted);
	UFUNCTION(BlueprintCallable)
	void NextAttack();
	UFUNCTION(BlueprintCallable)
	bool GetIsAttackAnimationPlaying();
	UFUNCTION(BlueprintPure)
	bool GetIsDodgeAnimationPlaying();
	TArray<AActor*> GetActorsFromSphere(float radius = 1200.0f, bool enableDebug = false);
	void PlayerAttack();

	//Enemy to focus camera or to attack closest to.
	class AEnemy* currEnemy;

	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value) override;

	//Soft locks to an enemy when not selected
	void SoftLock(float DeltaTime);
	
	//
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool bLeftCtrlButtonIsHeld;
	void LeftCTRLClick();

	void LeftCTRLClickEnd();

	void LeftMouseClick();

	void ShiftClick();
	
	UPROPERTY(EditAnywhere, Category = Dodge)
	TArray<UAnimMontage*> DodgeMontageArray;
	void PlayerDodgeEnd(UAnimMontage* animMontage, bool bInterrupted);
	UFUNCTION(BlueprintCallable)
	void NextDodge();
	void PlayerDodge();
	
	virtual FVector GetInputDirection() override;

private:

	UPROPERTY(EditAnywhere, Category = Combat)
	TMap<UAnimMontage*, UAnimMontage*> AttackMontageMap;
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<UAnimMontage*> FreeflowAttackMontageArray;
	UPROPERTY(EditAnywhere, Category = Combat)
	int currentAttackIndex = 0;
	bool playerCanAttck = true;
	bool playerCanDodge = true;

	class AEnemy* playerEnemy = nullptr;
};