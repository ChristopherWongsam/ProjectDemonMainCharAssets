// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectDemonCharacter.h"
#include "Containers/List.h"
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

UENUM(BlueprintType)
enum class EDemonMovementState :uint8
{
	MS_Normal UMETA(DisplayName = "Normal"),
	MS_Jump UMETA(DisplayName = "Jump"),
	MS_Glide UMETA(DisplayName = "Glide")
};
/**
 * 
 */
UCLASS()
class ADemonCharacter : public AProjectDemonCharacter
{
	GENERATED_BODY()

	class UMyMainCharacterAnimInstance* MainCharacterAnimInstance;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDemonMovementState MovementState;
	
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
	/** Right Mouse Action Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightMouseAction;


	UPROPERTY(EditAnywhere, Category = Mantle)
	UAnimMontage* MantleMontage;

	UPROPERTY(EditAnywhere, Category = Jump)
	UAnimMontage* JumpLandMontage;

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

	//Gets whether player attack animation is playing
	bool GetIsAttackAnimationPlaying();

	//Gets whether free flow attack animation is playing
	bool GetIsFreeflowAnimationPlaying();

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
	
	/// <summary>
	/// Setup user input
	/// </summary>
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void StartJump();

	virtual void StopJumping() override;

	
	/// <summary>
	/// Input functions
	/// </summary>
	
	bool bLeftCtrlButtonIsHeld;
	void LeftCTRLClick();
	void LeftCTRLClickEnd();
	void LeftMouseClick();
	void ShiftClick();
	void RightMouseClick();
	void RightMouseClickEnd();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsGliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BoostMaxTime = 2.0;

	float BoostTime = 0.0;
public:
	void ToggleBoost(bool reset, bool activate=true);
	void UpdateBoost(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, Category = Combat)
	float SwingSpeed = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEnableSwing = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bStartSwing = false;
	/// <summary>
	/// The Point of center where the character swings
	/// </summary>
	FVector OrbitPoint;
public:
	void Swing(float DeltaTime, bool enableDebug = false);
	void StartSwing();
	UFUNCTION()
	void EnableSwing();
private:
	bool playerCanDodge = true;

	UPROPERTY(EditAnywhere, Category = Dodge)
	TArray<UAnimMontage*> DodgeMontageArray;
public:
	void PlayerDodgeEnd(UAnimMontage* animMontage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void NextDodge();

	void PlayerDodge();

	/*Return whether chracter jumped or not*/
	UFUNCTION(BlueprintCallable)
	bool getJumpButtonisPressed();
	UFUNCTION(BlueprintCallable)
	void setJumpButtonisPressed(bool isPressed);
	/*Returns whether character landed on floor*/
	bool getCharacterLanded();

	void Landed(const FHitResult& Hit) override;

	

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BlueprintProtected = true))
	bool bCycleRunnigJumpMirror = false;

	bool bEnableHitBox = false;
	UFUNCTION(BlueprintCallable)
	void setEnableHitbox(bool enableHitbox);
	UFUNCTION(BlueprintCallable)
	void RestartHitbox();
	void StartHitbox(float deltaTime, bool bEnableRightPunch = true, bool enableDebug = false);

	void AttackHitbox(FName SocketName);
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactionMontage;

	/// <summary>
	/// Initiates how the character should react based on who is the sender
	/// </summary>
	/// <param name="sender">The actor who dealt the hit reaction</param>
	/// <returns></returns>
	float HitReact(AActor* hitSender);
	UFUNCTION()
	void HitReactEnd(UAnimMontage* animMontage, bool bInterrupted);

private:
	//Makesure for FName to seperate by ','
	UPROPERTY(EditAnywhere, Category = Combat)
	TMap<UAnimMontage*, FString> AttackMontageMap;

	UPROPERTY(EditAnywhere, Category = Combat)
	TMap<UAnimMontage*, FString> FreeflowAttackMontageMap;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* LauncherMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	int currentAttackIndex = 0;

	bool playerCanAttck = true;
	
	

	class AEnemy* playerEnemy = nullptr;
	TArray< AActor*> actorsHit;
	bool bJumpButtonIsPressed = false;
	bool bCharacterlanded = false;
	

	
};
