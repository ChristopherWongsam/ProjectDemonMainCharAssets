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
#include "Containers/Map.h"
#include "DemonCharacter.generated.h"

UENUM(BlueprintType)
enum class EDemonMovementState :uint8
{
	MS_Normal UMETA(DisplayName = "Normal"),
	MS_Jump UMETA(DisplayName = "Jump"),
	MS_Glide UMETA(DisplayName = "Glide")
};
UENUM(BlueprintType)
enum class ELowerArmState :uint8
{
	LAS_Normal UMETA(DisplayName = "Normal"),
	LAS_Mirror UMETA(DisplayName = "Mirror"),
	LAS_Disabled UMETA(DisplayName = "Disabled")
};
UENUM(BlueprintType)
enum class EUpperArmState :uint8
{
	LAS_Normal UMETA(DisplayName = "Normal"),
	LAS_Mirror UMETA(DisplayName = "Mirror"),
	LAS_Disabled UMETA(DisplayName = "Disabled")
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
	ADemonCharacter();
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
	/** E Press Action Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EPressAction;

	UPROPERTY(EditAnywhere, Category = Mantle)
	UAnimMontage* MantleMontage;

	UPROPERTY(EditAnywhere, Category = Jump)
	UAnimMontage* JumpLandMontage;

	UPROPERTY(EditAnywhere, Category = Jump)
	UAnimMontage* JumpStartMontage;

	UPROPERTY(EditAnywhere, Category = Mantle)
	float mantleZOffsset = 10;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	TArray<int> MaterialIndexHide;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Used for debugging of drawing the playewr input
	void DrawInput(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomMovement, meta = (AllowPrivateAccess = "true"))
	bool bEnableRun;

	void UpdateSpeed(float DeltaTime);
	
	float orignalWalkSpeed = 500.0;

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

	void UpdatePlayerAttack(float DeltaTime);

	// Returns distance frokm character with consideration of other characters capsule size;
	float getDistanceFromCharacter(ACharacter* character);

	UFUNCTION(BlueprintCallable)
	void PlayerAttackEnd(UAnimMontage* animMontage, bool bInterrupted);
	UFUNCTION(BlueprintCallable)
	void NextAttack();
	UFUNCTION(BlueprintCallable)

	//Gets whether player attack animation is playing
	bool GetIsAttackAnimationPlaying();

	//Gets whether free flow attack animation is playing
	bool GetIsFreeflowAnimationPlaying();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool GetIsDodgeAnimationPlaying();
	
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
	bool bRightMouseButtonIsPressed = false;
	void RightMouseClick();
	void RightMouseClickEnd();
	virtual void onMoveStarted(const FInputActionValue& Value);
	virtual void onMoveEnd(const FInputActionValue& Value) override;
	bool getIsRotationMontPlaying() const;

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
	void UpdateDodgeGlide(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void EndDodgeGlide();
	UPROPERTY(EditAnywhere, Category = Dodge)
	UAnimMontage* DodgeGlideStartMontage;
	UPROPERTY(EditAnywhere, Category = Dodge)
	float DodgeGlideSpeed = 1200.0;
	UPROPERTY(EditAnywhere, Category = Dodge)
	float DodgeGlideLength = 2.0;
	/*Return whether chracter jumped or not*/
	UFUNCTION(BlueprintCallable)
	bool getJumpButtonisPressed();
	UFUNCTION(BlueprintCallable)
	void setJumpButtonisPressed(bool isPressed);
	/*Returns whether character landed on floor*/
	bool getCharacterLanded();

	void Landed(const FHitResult& Hit) override;

	void UpdateHitbox(float deltaTime, bool bEnableRightPunch, bool enableDebug);

	

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BlueprintProtected = true))
	bool bCycleRunnigJumpMirror = false;

	void StartHitbox(float deltaTime, bool bEnableRightPunch = true, bool enableDebug = false);

	void UpdateHitbox(float deltaTime);

	void AttackHitbox(FName SocketName,bool bEnableDebug = false);
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<UAnimMontage*> HitReactionMontageArray;

	bool getHitReactionMontageIsActive() const;

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
	TArray<UAnimMontage*> AttackMontageArray;

	//Makesure for FName to seperate by ','
	UPROPERTY(EditAnywhere, Category = Combat)
	TMap<UAnimMontage*, FString> AttackMontageMap;

	bool bPlayerCanParry = false;

	//Chainmap sequence for block Or parrying
	UPROPERTY(EditAnywhere, Category = Combat)
	TMap<UAnimMontage*, UAnimMontage*> BlockParryChainMap;

	//Makesure for FName to seperate by ','
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<UAnimMontage*> BlockReactArray;

	//The minimum distance to stop attack rush
	UPROPERTY(EditAnywhere, Category = Combat)
	float attackRushMinimumDistance = 100;

	UFUNCTION()
	void AttackRushEnd(UAnimMontage* animMontage, bool bInterrupted);
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* AttackRushMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bEnableAttackRush = true;
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
	

public:
	void StartWallClimb();
	void EKeyActionPress();
	void UpdateWallclimb(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsClimbing = false;
private:
	
	FVector CLimbNormal;
private:
	bool bStartCharcterMovementRotation = false;
	float currentCharacterRotationTime = 0.0;
	float targetCurrentRotationTime;

	float characterInitialYawRotation;
	float characterFinalYawRotation;
public:
	void UpdateMovementRotation(float DeltatTime);

	UPROPERTY(BlueprintReadOnly)
	ELowerArmState LowerArmState = ELowerArmState::LAS_Normal;

	UPROPERTY(BlueprintReadOnly)
	EUpperArmState UpperArmState = EUpperArmState::LAS_Normal;


	UFUNCTION(BlueprintCallable)
	EDemonMovementState GetMovementState() { return MovementState; }

};
