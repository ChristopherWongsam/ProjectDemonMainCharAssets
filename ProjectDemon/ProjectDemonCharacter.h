// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ProjectDemonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProjectDemonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	AProjectDemonCharacter();
	

protected:
	/**Handles timer for class */
	FTimerHandle timerHandler;

	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Delay(float duration, FName funcName);

	void ResetCollision();
			
	TArray<AActor*> actorsToIgnore;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void onMoveEnd(const FInputActionValue& Value);
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MovingForwardValue = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MovingRightValue = 0;

public:
	//Getters

	/** Returns any movment input is received **/
	bool getMovementInputReceived();

	bool bInAir() const;

	/** This will initiate the jump animation **/
	void StartJumping() ;

	UFUNCTION()
		void launchCharacterUp();

	void Log(FString log, bool printToScreen = true);
	float PlayMontage(UAnimMontage* Montage, FName Section = "Default", float rate = 1.0, bool bEnaleLowerArmAnim = false);
	bool SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, TArray<AActor*> ActorsToIgnore, int trace, FHitResult& HitResult, bool traceComplex = false, bool ignoreSelf = true);
	bool SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, FHitResult& HitResult, bool traceComplex = false, bool ignoreSelf = true);

	bool SphereTraceMulti(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, TArray<FHitResult>& HitResults, bool traceComplex = false, bool ignoreSelf = true);


	
	//Will move and rotate the character over a certain time
	void MoveCharacterToRotationAndLocationIninterval(FVector TargetRelativeLocation, FRotator TargetRelativeRotation, float OverTime);

	virtual FVector GetInputDirection();

	//Will return the time in anim sequence to get the time. If it cannot find it it will return -1.0
	UFUNCTION()
	float getMontageAnimNotifyTime(const UAnimMontage* Mont, FString notifyName, FString notifyPrefix = "AnimNotify_");

};

