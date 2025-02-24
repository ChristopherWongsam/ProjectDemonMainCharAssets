// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


UENUM(BlueprintType)
enum class ETurnState :uint8
{
	ETS_Left UMETA(DisplayName = "Left"),
	ETS_Right UMETA(DisplayName = "Right"),
	ETS_None UMETA(DisplayName = "None"),
	ETS_RightHalf UMETA(DisplayName = "RightHalf"),
	ETS_LeftHalf UMETA(DisplayName = "LeftHalf")
};
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

	//Getters
	float getSpeed();

	bool InAir() const;

	bool CanHitReact = true;

	/** Plays the specified effect at the given location and rotation, fire and forget. The system will go away when the effect is complete. Does not replicate.
	 * @param particleSystem - particle system to create
	 * @param Location - location to place the effect in world space
	 * @param Rotation - rotation to place the effect in world space
	 * @param Scale - scale to create the effect at
	 */
	void SpawnParticle(UParticleSystem* particleSystem, FVector Location, FRotator Rotation, FVector SpawnScale = FVector(1,1,1));
	virtual void Log(FString log, bool printToScreen = true);
	/** Modifies log screen color**/
	virtual void LogScreen(FString log, FLinearColor color = FLinearColor::Blue);
	void PrintLog(FString log);
	/*Custom way to play anim montage.Should use this.If there is no valid montage will return -1.0.*/
	virtual float PlayMontage(UAnimMontage* Montage, FName Section = "Default", float rate = 1.0, bool bEnaleLowerArmAnim = false);
	/*Binds montage to function oncee montage is done. Remeber to make functions UFUNCTION()*/ 
	void BindMontage(UAnimMontage* Montage, FName functionName);
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

	//Will cancel move and rotate the character over a certain time
	void cancelMoveCharacterToRotationAndLocationIninterval();

	//Will turn in place towarsds the yaw CameraRotation. Can be any vector really, but this function was ideally designed for camera but can be use for other scenario's
	ETurnState TurnInPlace(FRotator CameraRotation);
	
	//Interps a value using time
	float InterpValueTime(float currentValue, float targetValue, float DeltaTime);

	//Will return the time in anim sequence to get the time. If it cannot find it it will return -1.0
	UFUNCTION()
	float getMontageAnimNotifyTime(const UAnimMontage* Mont, FString notifyName, FString notifyPrefix = "AnimNotify_");

	void Delay(float duration, FName funcName);
	void CancelAllDelay();
	/*Sets whether movment input can cancel the montage.Recommonded to use with anim notify.*/
	UFUNCTION(BlueprintCallable)
	void setCanCancelAnimMontage(bool canCancelAnimMontage = true);
	/*Returns whether the animation can be canceled.*/
	bool getCanCancelAnimMontage();

	/*Sets whether character can be hit reacted. Set to true if during montage, the montage can be interrupted by hit reaction*/
	UFUNCTION(BlueprintCallable)
	void setCanHitReact(bool canCancelAnimMontage = true);
	/*Gets whether character can be hit reacted. If true, the montage can be interrupted when hit, false the player cannot be interrupted*/
	bool getCanHitReact() { return bCanCancelAnimMontage; };

	void ResetMovementComponentValues();

	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightHandSocketName = "RightHandSocket";
	UPROPERTY(EditAnywhere, Category = Combat)
	FName LeftHandSocketName = "LeftHandSocket";
	UPROPERTY(EditAnywhere, Category = Combat)
	FName RightFootSocketName = "RightFootSocket";
	UPROPERTY(EditAnywhere, Category = Combat)
	FName LeftFootSocketName = "LeftFootSocket";

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitImpact;
	UPROPERTY(EditAnywhere, Category = CombatVFX)
	class UNiagaraSystem* HitNiagraImpact;
	UPROPERTY(EditAnywhere, Category = CombatVFX)
	float hitImpactSize = 0.5;
protected:
	/**For traces */
	TArray<AActor*> actorsToIgnore;
	/**Handles timer for class */
	FTimerHandle timerHandler;

	float defaultGravityScale;
	float defaultAirControl;

	FName AttackSocketName = "RightHandSocket";
public:
	//Enables hit box. Will only activate if 
	UFUNCTION(BlueprintCallable)
	void setEnableHitbox(bool enableHitbox);

	// if set to false it will disable hit box
	UFUNCTION(BlueprintCallable)
	void setEnableLimbHitbox(bool enableHitbox, FName LimbAttackSocketName = "RightHandSocket");

	UFUNCTION(BlueprintCallable)
	void RestartHitbox();
	TArray< AActor*> actorsHit;
	bool bEnableHitBox = false;
	bool bEnableLimbHitBox = false;
protected:
	FName limbAttackSocketName = "RightHandSocket";
};
