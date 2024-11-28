// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyMainCharacterAnimInstance.generated.h"
class ADemonCharacter;
/**
 * 
 */
UCLASS()
class  UMyMainCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void manageLimbLookAt(float DeltaTime);
	void SetLeftFootEffectorLocation(FVector NewEffectorLocation);
	bool isPlayingStopinMontaging;
	void SetRightFootEffectorLocation(FVector NewEffectorLocation);
	void SetLeftFootAlpha(float NewAlpha);
	void SetRightFootAlpha(float NewAlpha);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bIsMoving;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bIsFalling;
	UFUNCTION(BlueprintPure)
		FVector CalculateLaunchVelocity(UAnimSequence* Sequence);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		ADemonCharacter* MyChar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool ShouldCharcterFall = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bInGrappleState = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool ReachedBottomOfArk = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimSequence* GrappleAirStateAnimSeq;
	/// <summary>
	/// Include notify named JumpStart and JumpEnd in AnimSequence to work.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* JumpStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* JumpLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* JumpLand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool Dead = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float ProjectedTimeInAir = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float HipOffset = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float ForwardValue = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float SideValue = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FVector IK_RightHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool InGrapple = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bGrappleToEnemy = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FRotator RotationToAddToArm = FRotator(0);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FVector IK_LeftHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FVector LimbLookAtLocation = FVector(0);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bEnableLimbLookAt = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		bool bEnableRightArmLookAt = false;
	bool* bEnableRightArmLookAtPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		bool bEnableLefttArmLookAt = false;
	bool* bEnableLefttArmLookAtPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		bool bEnableRightLegLookAt = false;
	bool* bEnableRightLegLookAtPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		bool bEnableLeftLegLookAt = false;
	bool* bEnableLeftLegLookAtPtr;
	/** IK Location for left foot for match floor */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector LeftFootIKLocation;

	/** IK Location for right foot for match floor */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector RightFootIKLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector LeftFootEffectorLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector JointTargetLeft;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		float LeftFootAlpha;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		float YawDelta = 0.0;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isAccelerating;
	FRotator RotationLastTick = FRotator::ZeroRotator;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector RightFootEffectorLocation;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		FVector RighArmLookAtLocation;
	FVector* RighArmLookAtLocationPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		FVector LeftLegLookAtLocation;
	FVector* LeftLegLookAtLocationPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		FVector RightLegLookAtLocation;
	FVector* RightLegLookAtLocationPtr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LimbLookAt, meta = (BlueprintProtected = true))
		FVector LeftArmLookAtLocation;
	FVector* LeftArmLookAtLocationPtr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector JointTargetRight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		float RightFootAlpha;
	float PrevDirection;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Character|IK")
		bool CalculateDirectionStateArea;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Character|IK")
		bool Turn180;
	
	/** IK Rotation for left foot for match floor surface */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FRotator LeftFootIKRotation;

	/** IK Rotation for right foot for match floor surface */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FRotator RightFootIKRotation;

	/** IK Hip offset vector only work in Z direction */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
		FVector HipOffsetVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyInstance, meta = (BlueprintProtected = true))
		bool CanHang = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyInstance, meta = (BlueprintProtected = true))
		bool EnableIk;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float JumpScalar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float AngleBetPlayerAndForwardVect;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float Speed = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool IsMoving = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool InAir = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float Direction = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float Pitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float Roll;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float Yaw = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float BoneRotationValues = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Limb, meta = (BlueprintProtected = true))
		bool bDisableUpperArm = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Limb, meta = (BlueprintProtected = true))
		bool bDisableLowerArm = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FRotator SpineRotation;
	void AimOffset(const FVector& Loc, const FRotator& Rot,FName Bone = "Spine1");
	void LimbLookAt(FVector Target = FVector::ZeroVector);
	void LimbLookAt(FName BoneName = "RightForeArmStretch");
	void EnableRightArmLookAt(bool enableLookAt = true);


	float pauseLength = 1.0;
	void setPauseLength(float PauseLength) { pauseLength = PauseLength; }
	//By default the pause length is 5
	float getPauseLength() { return pauseLength; }
	FTimerHandle PauseAnimMontageTimer;
	UAnimMontage* CurrentPausedMontage;
	bool bEnablePauseMontage = false;
	bool bEnableFloatDown = false;
	float montageCurrPlayRate;
	float montageOrigPlayRate;
	/// 
	/// Used to pause anim for a given length
	/// 
	/// <param name="length">How long to pause</param>
	/// <param name="allowFunctionOverride">Use the length that is passed into the function or not</param>
	UFUNCTION(BlueprintCallable)
		float PauseAnimMontage(float length = 1.0, bool allowFunctionOverride = true);
	UFUNCTION(BlueprintCallable)
		void ResumePausedAnimMontage();
	UFUNCTION(BlueprintCallable)
		void SlowAnimMont(float rate);
	UFUNCTION(BlueprintCallable)
		void ResumeMont(float rate=1.0);
	UFUNCTION(BlueprintCallable)
		void DisableLimbLookAt();

	UFUNCTION(BlueprintCallable)
		void startBlink();

	/** Gets the anim notify time from anim sequence **/
	UFUNCTION(BlueprintCallable)
		float getAnimNotifyTime(UAnimSequence* animSequence, FString notifyNmae, FString notifyPrefix = "AnimNotify_");
	float blinkVal = 0.0;
	FTimerHandle blinkTimer;

	
	APawn* Owner;


};
