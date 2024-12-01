// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class PROJECTDEMON_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	void Log(FString log, bool printToScreen = true);
	/** Modifies log screen color**/
	void LogScreen(FString log, FLinearColor color = FLinearColor::Blue);
	void PrintLog(FString log);
	float PlayMontage(UAnimMontage* Montage, FName Section = "Default", float rate = 1.0, bool bEnaleLowerArmAnim = false);
	bool SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, TArray<AActor*> ActorsToIgnore, int trace, FHitResult& HitResult, bool traceComplex = false, bool ignoreSelf = true);
	bool SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, FHitResult& HitResult, bool traceComplex = false, bool ignoreSelf = true);
	bool SphereTraceMulti(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, TArray<FHitResult>& HitResults, bool traceComplex = false, bool ignoreSelf = true);
	//Will move and rotate the character over a certain time
	void MoveCharacterToRotationAndLocationIninterval(FVector TargetRelativeLocation, FRotator TargetRelativeRotation, float OverTime);

	//Will return the time in anim sequence to get the time. If it cannot find it it will return -1.0
	UFUNCTION()
	float getMontageAnimNotifyTime(const UAnimMontage* Mont, FString notifyName, FString notifyPrefix = "AnimNotify_");

	void Delay(float duration, FName funcName);

protected:
	/**For traces */
	TArray<AActor*> actorsToIgnore;
	/**Handles timer for class */
	FTimerHandle timerHandler;
};
