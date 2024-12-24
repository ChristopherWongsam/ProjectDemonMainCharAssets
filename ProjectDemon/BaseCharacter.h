// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class PROJECTDEMON_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	bool bCanCancelAnimMontage = false;
	float TimeInterpValue = 0.0;
	float newValueFromChange(float value, float newValue);
public:
	virtual void BeginPlay() override;
	void Log(FString log, bool printToScreen = true);
	/** Modifies log screen color**/
	void LogScreen(FString log, FLinearColor color = FLinearColor::Blue);
	void PrintLog(FString log);
	//Custom way to play anim montage. Should use this. If there is no valid montage will return -1.0.
	float PlayMontage(UAnimMontage* Montage, FName Section = "Default", float rate = 1.0, bool bEnaleLowerArmAnim = false);
	/*
	* 
	* Trace int
	*	 0.None, 
	*	1.ForOneFrame, 
	*	2.ForDuration, 
	*	3.Persistent
	*/
	bool SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, TArray<AActor*> ActorsToIgnore, int trace, FHitResult& HitResult, bool traceComplex = false, bool ignoreSelf = true);
	/*
	*
	* Trace int
	*	 0.None,
	*	1.ForOneFrame,
	*	2.ForDuration,
	*	3.Persistent
	*/
	bool SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, FHitResult& HitResult, bool traceComplex = false, bool ignoreSelf = true);
	/*
	*
	* Trace int
	*	 0.None,
	*	1.ForOneFrame,
	*	2.ForDuration,
	*	3.Persistent
	*/
	bool SphereTraceMulti(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, TArray<FHitResult>& HitResults, bool traceComplex = false, bool ignoreSelf = true);
	//Will move and rotate the character over a certain time
	void MoveCharacterToRotationAndLocationIninterval(FVector TargetRelativeLocation, FRotator TargetRelativeRotation, float OverTime);

	//Will move and rotate the character over a certain time
	void cancelMoveCharacterToRotationAndLocationIninterval();


	
	//Interps a value using time
	float InterpValueTime(float currentValue, float targetValue, float DeltaTime);

	//Will return the time in anim sequence to get the time. If it cannot find it it will return -1.0
	UFUNCTION()
	float getMontageAnimNotifyTime(const UAnimMontage* Mont, FString notifyName, FString notifyPrefix = "AnimNotify_");

	void Delay(float duration, FName funcName);
	/*Sets whether movment input can cancel the montage.Recommonded to use with anim notify.*/
	UFUNCTION(BlueprintCallable)
	void setCanCancelAnimMontage(bool canCancelAnimMontage = true);
	/*Returns whether the animation can be canceled.*/
	bool getCanCancelAnimMontage();

	void ResetMovementComponentValues();

	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightHandSocketName = "RightHandSocket";

	UPROPERTY(EditAnywhere, Category = Combat)
	FName LeftHandSocketName = "LeftHandSocket";

	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightFootSocketName = "RightFootSocket";

	UPROPERTY(EditAnywhere, Category = Combat)
	FName LeftFootSocketName = "LeftFootSocket";


protected:
	/**For traces */
	TArray<AActor*> actorsToIgnore;
	/**Handles timer for class */
	FTimerHandle timerHandler;

	float defaultGravityScale;
	float defaultAirControl;
};
