// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectDemonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Runtime/Engine/Private/InterpolateComponentToAction.h>
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AProjectDemonCharacter

AProjectDemonCharacter::AProjectDemonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AProjectDemonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectDemonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		// 
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectDemonCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AProjectDemonCharacter::onMoveEnd);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectDemonCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AProjectDemonCharacter::onMoveEnd(const FInputActionValue& Value)
{
	Log("Move ended.");
	MovingForwardValue = 0.0;
	MovingRightValue = 0.0;

	isRotationSpeedSet = false;
}
void AProjectDemonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovingForwardValue = MovementVector.Y;
	MovingRightValue = MovementVector.X;
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		//AddMovementInput(ForwardDirection, MovementVector.Y);
		//AddMovementInput(RightDirection, MovementVector.X);
		MoveForward(MovementVector.Y);
		MoveRight(MovementVector.X);
	}
}

void AProjectDemonCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProjectDemonCharacter::MoveForward(float Value)
{
	
	float ViewController = Controller->GetControlRotation().Yaw;;
	if (Value != 0.0)
	{
		auto YawRoration = FRotator(0.0, ViewController, 0.0);
		auto newDirection = FRotationMatrix(YawRoration).GetUnitAxis(EAxis::X);
		auto otherdir = UKismetMathLibrary::MakeRotator(0, 0, GetActorRotation().Yaw);
		auto dir = Value * newDirection + UKismetMathLibrary::GetForwardVector(otherdir) * MoveForwardStrength;

		AddMovementInput(dir, 1);
	}
}

void AProjectDemonCharacter::MoveRight(float Value)
{
	float ViewController = Controller->GetControlRotation().Yaw;;
	if (Value != 0.0)
	{
		auto YawRoration = FRotator(0.0, ViewController, 0.0);
		auto newDirection = 1 * FRotationMatrix(YawRoration).GetUnitAxis(EAxis::Y);
		auto otherdir = UKismetMathLibrary::MakeRotator(0, 0, GetActorRotation().Yaw);
		auto dir = Value * newDirection + UKismetMathLibrary::GetForwardVector(otherdir) * MoveForwardStrength;
		
		AddMovementInput(dir, 1);
	}
}
bool AProjectDemonCharacter::getMovementInputReceived()
{
	return MovingForwardValue != 0.0 || MovingRightValue != 0.0;
}
void AProjectDemonCharacter::launchCharacterUp()
{
	Super::Jump();
}
void AProjectDemonCharacter::ResetCollision()
{
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCameraBoom()->bDoCollisionTest = true;
}

void AProjectDemonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

FVector AProjectDemonCharacter::GetInputDirection()
{
	auto followCamera = GetFollowCamera();
	auto rot = UKismetMathLibrary::MakeRotator(0, 0, GetFollowCamera()->GetComponentRotation().Yaw);
	auto forwVect = UKismetMathLibrary::GetForwardVector(rot);
	auto rightVector = UKismetMathLibrary::GetRightVector(rot);
	auto ForwardVect = forwVect * MovingForwardValue;
	auto RightVect = rightVector * MovingRightValue;
	auto InputDirection = ForwardVect + RightVect;
	InputDirection.Normalize();

	return InputDirection;
}
void AProjectDemonCharacter::UpdateCamera(float DeltaTime)
{
	if (InAir() && GetVelocity().X == 0.0 && GetVelocity().Y == 0.0)
	{
		GetCameraBoom()->CameraLagSpeed = CameraLagAirSpeed;
	}
	else
	{
		GetCameraBoom()->CameraLagSpeed = CameraLagFloorSpeed;
	}
	if (getSpeed() != 0.0)
	{
		float Val = 0.0;

		FRotator ControlRotation(0.0, GetControlRotation().Yaw, 0.0);

		FVector ControlRotationForwardVector = UKismetMathLibrary::GetForwardVector(ControlRotation);

		float dotProduct = FVector::DotProduct(ControlRotationForwardVector, GetInputDirection());
		dotProduct = FMath::Abs(dotProduct);

		dotProduct = 1 - dotProduct;

		float power = FMath::Pow(dotProduct, cameraToPlayerSpeed);

		float totalMovingDirections = FMath::Abs(MovingForwardValue) + FMath::Abs(MovingRightValue);
		float clampedTotalMovingDirections = UKismetMathLibrary::FClamp(totalMovingDirections, 0.0, 1.0);
		float clampedDeltaTime = UKismetMathLibrary::FClamp(DeltaTime, 0.0, 1.0);

		Val = power * clampedTotalMovingDirections * clampedDeltaTime * cameraRotationRate * UKismetMathLibrary::NormalizedDeltaRotator(GetInputDirection().Rotation(), GetControlRotation()).Yaw;

		AddControllerYawInput(Val);
	}
	
	
}