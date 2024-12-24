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
	bool isPlayingStopinMontaging;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bIsMoving;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bIsFalling;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bJumpButtonIsPressed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bCharacterLanded;
	UFUNCTION(BlueprintPure)
		FVector CalculateLaunchVelocity(UAnimSequence* Sequence);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		ADemonCharacter* MyChar = nullptr;

	
	/// <summary>
	/// Include notify named JumpStart and JumpEnd in AnimSequence to work.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* JumpRun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* JumpStill;
	


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool isAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyInstance, meta = (BlueprintProtected = true))
		bool CanHang = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyInstance, meta = (BlueprintProtected = true))
		bool EnableIk;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		float Speed = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bRunJumpCycle = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FRotator SpineRotation;
	void AimOffset(const FVector& Loc, const FRotator& Rot,FName Bone = "Spine1");


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
		void startBlink();
	UFUNCTION(BlueprintCallable)
		void Blink();
	/** Gets the anim notify time from anim sequence **/
	UFUNCTION(BlueprintCallable)
		float getAnimNotifyTime(UAnimSequence* animSequence, FString notifyNmae, FString notifyPrefix = "AnimNotify_");
	float blinkVal = 0.0;
	FTimerHandle blinkTimer;
	bool bEyesClosed = false;
	float currentBlinkValue = 0.0;
	
	APawn* Owner;


};
