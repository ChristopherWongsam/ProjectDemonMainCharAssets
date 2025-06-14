// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "MyMainCharacterAnimInstance.generated.h"
class ADemonCharacter;
/**
 * 
 */
UCLASS()
class  UMyMainCharacterAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()
	


public:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	bool isPlayingStopinMontaging;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bIsMoving;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		bool bRunJumpCycle = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyInstance, meta = (BlueprintProtected = true))
		FRotator SpineRotation;
	void AimOffset(const FVector& Loc, const FRotator& Rot,FName Bone = "Spine1");


	
	UFUNCTION(BlueprintCallable)
	void SetEnableMorphTargets(TMap<FName,float> MorphTargetNameMap, TMap<FName, bool> MorphEnableOscillateMap);

	
	UFUNCTION(BlueprintCallable)
		void startBlink();
	UFUNCTION(BlueprintCallable)
		void Blink();

	void UpdateBlink(float DeltaTime);

	bool bEnableMirror = false;
	UFUNCTION(BlueprintCallable)
		void setEnableMirror(bool EnableMirror);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool getEnableMirror();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BlinkCurve = "EyesClosedCurve";
	float maxBlinkVal = 1.0;
	FTimerHandle blinkTimer;
	bool bEyesClosed = false;
	float currentBlinkValue = 0.0;
	
};
