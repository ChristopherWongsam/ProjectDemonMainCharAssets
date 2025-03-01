// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacter.h"
#include "BaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDEMON_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
	bool bIsFalling;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
	float Speed = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
	float Direction = 0.0f;

	void Log(FString log, bool printToScreen = true);

	UFUNCTION(BlueprintCallable)
	FVector getRootMotionDataArray(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	FVector getRootMotionDataAtFrame(UAnimMontage* Montage, int frame);

	FVector getRootMotionDataFromActiveMontage(float time);

	FVector getRootMotionDataFromActiveMontage();

	/** Gets the anim notify time from anim sequence **/
	UFUNCTION(BlueprintCallable)
	float getAnimNotifyTime(UAnimSequence* animSequence, FString notifyNmae, FString notifyPrefix = "AnimNotify_");

private:
	UAnimMontage* RootMotionMontage;
	bool bEnableRootMotionModifier = false;
	FVector AnimCharOriginalLocation;
	FVector AnimCharForwardVector;
	FVector AnimCharRightVector;
	FVector AnimCharUpVector;
	FVector rootMotionVectorScale;

public:
	void bindMontageRootMotionModifier(UAnimMontage* Montage, float rootMotionScale);
	void bindMontageRootMotionModifier(UAnimMontage* Montage, FVector vectorScale = FVector(1));
	void UpdateMontageRootMotion(float DeltaTime);

	
private:
	FTimerHandle PauseAnimMontageTimer;
	UAnimMontage* CurrentPausedMontage;
	bool bEnablePauseMontage = false;
	bool bEnableFloatDown = false;
	float montageCurrPlayRate;
	float montageOrigPlayRate;
	float pauseLength = 1.0;
public:
	/// 
	/// Used to pause anim for a given length
	/// 
	/// <param name="length">How long to pause</param>
	/// <param name="allowFunctionOverride">Use the length that is passed into the function or not</param>
	UFUNCTION(BlueprintCallable)
	float PauseAnimMontage(float length = 1.0, float playRate = 0.2, bool allowFunctionOverride = true);
	UFUNCTION(BlueprintCallable)
	void ResumePausedAnimMontage();
	UFUNCTION(BlueprintCallable)
	void SlowAnimMont(float rate);
	UFUNCTION(BlueprintCallable)
	void ResumeMont(float rate = 1.0);

	void setPauseLength(float PauseLength) { pauseLength = PauseLength; }
	//By default the pause length is 5
	float getPauseLength() { return pauseLength; }

private:
	ABaseCharacter* AnimCharacter = nullptr;

public:
	ABaseCharacter* GetAnimCharacter() { return AnimCharacter; }

};
