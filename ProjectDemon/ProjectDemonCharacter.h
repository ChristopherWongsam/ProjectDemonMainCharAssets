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
	//Getters

	float getSpeed();

	/** Returns any movment input is received **/
	bool getMovementInputReceived();

	bool InAir() const;

	UFUNCTION()
	void launchCharacterUp();

	virtual FVector GetInputDirection();

public:
	UPROPERTY(EditAnywhere, Category = Movement)
	UAnimMontage* TurnRightMontage;
	UPROPERTY(EditAnywhere, Category = Movement)
	UAnimMontage* TurnBackMontage;

	bool isRotationSpeedSet = false;
};

