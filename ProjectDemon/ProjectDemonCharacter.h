// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Logging/LogMacros.h"
#include "ProjectDemonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProjectDemonCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
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
	

public:
	UPROPERTY(EditAnywhere, Category = Movement)
	float MoveForwardStrength = 1.0;
protected:
	

	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	void MoveForward(float Value);

	void MoveRight(float Value);

	void ResetCollision();

	void Landed(const FHitResult& Hit) override;
			
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
	

	/** Returns any movment input is received **/
	bool getMovementInputReceived();

	

	UFUNCTION()
	void launchCharacterUp();

	virtual FVector GetInputDirection();

public:

	UPROPERTY(EditAnywhere, Category = Movement)
	UAnimMontage* TurnRightMontage;
	UPROPERTY(EditAnywhere, Category = Movement)
	UAnimMontage* TurnLeftMontage;
	UPROPERTY(EditAnywhere, Category = Movement)
	UAnimMontage* TurnBackLeftMontage;
	UPROPERTY(EditAnywhere, Category = Movement)
	UAnimMontage* TurnBackRightMontage;

	UPROPERTY(EditAnywhere, Category = Movement)
	FName TurnCurveName = "CharacterRotationCurve";
	UPROPERTY(EditAnywhere, Category = Movement)
	FName MoveSpeedData = "MoveSpeedData";

	bool isRotationSpeedSet = false;


	// WIll adjust how the camera returns to normal when moving
	UPROPERTY(EditAnywhere, category = "Camera")
	float cameraToPlayerSpeed = 0.5;

	// The control factor of auto camera rotation.
	UPROPERTY(EditAnywhere, category = "Camera")
	float cameraRotationRate = 0.6;

	// Camera lag speed when player is walking.
	UPROPERTY(EditAnywhere, category = "Camera")
	float CameraLagFloorSpeed = 10.0;

	// Camera lag speed when player is in Air Jumping.
	UPROPERTY(EditAnywhere, category = "Camera")
	float CameraLagAirSpeed = 0.2;

	void UpdateCamera(float DeltaTime);

	float springArmSideLength = 0;
	float springArmForwardLength = 0;
	float springArmUpLength = 0;

	float springArmSpeed = 50;

	UPROPERTY(EditAnywhere, category = "Camera")
	float idleSpringArmDelay = 10;
	UPROPERTY(EditAnywhere, category = "Camera")
	float idleSpringArmSpeed = 10;
	UPROPERTY(EditAnywhere, category = "Camera")
	float movementSpringArmSpeed = 50;
	UPROPERTY(EditAnywhere, category = "Camera")
	float idleSpringArmSideLength = 50;
	UPROPERTY(EditAnywhere, category = "Camera")
	float idleSpringArmForwardLength = 300;
	UPROPERTY(EditAnywhere, category = "Camera")
	float idleSpringArmUpLength = 100;
	//USpringArmComponent SpringArmComp;
	void UpdateCameraArmLength(float DeltaTime);

	UFUNCTION()
	void EnableZoomToChar();
};

