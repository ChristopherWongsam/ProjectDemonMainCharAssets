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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AProjectDemonCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

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
		auto dir = Value * newDirection + UKismetMathLibrary::GetForwardVector(otherdir);
		AddMovementInput(dir, 1);
	}
}

void AProjectDemonCharacter::MoveRight(float Value)
{
	MovingRightValue = Value;
	float ViewController = Controller->GetControlRotation().Yaw;;
	if (Value != 0.0)
	{
		auto YawRoration = FRotator(0.0, ViewController, 0.0);
		auto newDirection = 1 * FRotationMatrix(YawRoration).GetUnitAxis(EAxis::Y);
		auto otherdir = UKismetMathLibrary::MakeRotator(0, 0, GetActorRotation().Yaw);
		auto dir = Value * newDirection;
		dir = Value * newDirection + UKismetMathLibrary::GetForwardVector(otherdir);

		AddMovementInput(dir, 1);
	}
}

bool AProjectDemonCharacter::getMovementInputReceived()
{
	return GetCharacterMovement()->Velocity.Size() > 0.0;
}

bool AProjectDemonCharacter::bInAir() const
{
	return GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling || GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying;
}

void AProjectDemonCharacter::StartJumping()
{
	Log("Jump button pressed.");
}

void AProjectDemonCharacter::launchCharacterUp()
{
	Super::Jump();
}
void AProjectDemonCharacter::Delay(float duration, FName funcName)
{
	FTimerHandle TimerHandle_AttackDelay;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, funcName);

	GetWorld()->GetTimerManager().SetTimer(
		timerHandler, // handle to cancel timer at a later time
		Delegate, // function to call on elapsed
		duration, // float delay until elapsed
		false); // looping?
}

void AProjectDemonCharacter::ResetCollision()
{
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCameraBoom()->bDoCollisionTest = true;
}

void AProjectDemonCharacter::Log(FString log, bool printToScreen)
{
	UKismetSystemLibrary::PrintString(this, log, printToScreen);
}

float AProjectDemonCharacter::getMontageAnimNotifyTime(const UAnimMontage* Mont, FString notifyNmae, FString notifyPrefix)
{
	auto Time = -1.0;
	bool animFound = false;
	if (!Mont)
	{
		Log( "Did not find AnimNotify: " + notifyNmae);
		return Time;
	}
	for (auto animNotify : Mont->AnimNotifyTracks)
	{
		for (auto notify : animNotify.Notifies)
		{
			if (notify->GetNotifyEventName() == notifyPrefix + notifyNmae)
			{
				Log("Found AnimNotify: " + notifyNmae);
				Time = (notify->GetTime());
				animFound = true;
			}
		}
	}
	if (!animFound)
	{
		Log("Anim Not Found");
	}
	return Time;
}

float AProjectDemonCharacter::PlayMontage(UAnimMontage* Montage, FName Section, float rate, bool bEnaleLowerArmAnim)
{
	if (Montage)
	{
		Log("Valid montage!");
		auto T = GetMesh()->GetAnimInstance()->Montage_Play(Montage, rate);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(Section, Montage);
		auto time = Montage->GetSectionLength(Montage->GetSectionIndex(Section));
		time -= Montage->GetDefaultBlendOutTime();
		float Rate = Montage->RateScale;
		return time / Rate;
	}
	else
	{
		Log("Invalid montage!");
		return -1.0;
	}
}

bool AProjectDemonCharacter::SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, TArray<AActor*> ActorsToIgnore, int trace, FHitResult& HitResult, bool traceComplex, bool ignoreSelf)
{
	return UKismetSystemLibrary::SphereTraceSingle(this, StartPoint, EndPoint, sphereRadius, traceTypeQuery,
		traceComplex, ActorsToIgnore, (EDrawDebugTrace::Type)trace, HitResult, ignoreSelf);
}
bool AProjectDemonCharacter::SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, FHitResult& HitResult, bool traceComplex, bool ignoreSelf)
{
	return UKismetSystemLibrary::SphereTraceSingle(this, StartPoint, EndPoint, sphereRadius, traceTypeQuery,
		traceComplex, actorsToIgnore, (EDrawDebugTrace::Type)trace, HitResult, ignoreSelf);
}
bool AProjectDemonCharacter::SphereTraceMulti(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, TArray<FHitResult> &HitResults, bool traceComplex, bool ignoreSelf)
{
		return UKismetSystemLibrary::SphereTraceMulti(this, StartPoint, EndPoint, sphereRadius, traceTypeQuery,
			traceComplex, actorsToIgnore, (EDrawDebugTrace::Type)trace, HitResults, ignoreSelf);
}
void AProjectDemonCharacter::MoveCharacterToRotationAndLocationIninterval(FVector TargetLocation, FRotator TargetRotation, float OverTime)
{

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	auto World = GetWorld();
	auto Component = GetRootComponent();
	bool bEaseIn = true;
	bool bEaseOut = true;
	auto bForceShortestRotationPath = false;
	TEnumAsByte<EMoveComponentAction::Type> MoveComponentAction = EMoveComponentAction::Move;
	Log("MoveCharacterToRotationAndLocationIninterval called");
	if (World)
	{
		Log("World found");
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		FInterpolateComponentToAction* Action = LatentActionManager.FindExistingAction<FInterpolateComponentToAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

		const FVector ComponentLocation = (GetRootComponent() != NULL) ? GetRootComponent()->GetRelativeLocation() : FVector::ZeroVector;
		const FRotator ComponentRotation = (GetRootComponent() != NULL) ? GetRootComponent()->GetRelativeRotation() : FRotator::ZeroRotator;

		// If not currently running
		if (Action == NULL)
		{
			Log("Action not found", false);
			if (MoveComponentAction == EMoveComponentAction::Move)
			{
				// Only act on a 'move' input if not running
				Action = new FInterpolateComponentToAction(OverTime, LatentInfo, Component, bEaseOut, bEaseIn, bForceShortestRotationPath);

				Action->TargetLocation = TargetLocation;
				Action->TargetRotation = TargetRotation;

				Action->InitialLocation = ComponentLocation;
				Action->InitialRotation = ComponentRotation;

				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
			}
		}
		else
		{
			Log("Action found", false);
			if (MoveComponentAction == EMoveComponentAction::Move)
			{
				// A 'Move' action while moving restarts interpolation
				Action->TotalTime = OverTime;
				Action->TimeElapsed = 0.f;

				Action->TargetLocation = TargetLocation;
				Action->TargetRotation = TargetRotation;

				Action->InitialLocation = ComponentLocation;
				Action->InitialRotation = ComponentRotation;
			}
			else if (MoveComponentAction == EMoveComponentAction::Stop)
			{
				// 'Stop' just stops the interpolation where it is
				Action->bInterpolating = false;
			}
			else if (MoveComponentAction == EMoveComponentAction::Return)
			{
				// Return moves back to the beginning
				Action->TotalTime = Action->TimeElapsed;
				Action->TimeElapsed = 0.f;

				// Set our target to be our initial, and set the new initial to be the current position
				Action->TargetLocation = Action->InitialLocation;
				Action->TargetRotation = Action->InitialRotation;

				Action->InitialLocation = ComponentLocation;
				Action->InitialRotation = ComponentRotation;
			}
		}
	}
	//UKismetSystemLibrary::MoveComponentTo(GetRootComponent(), TargetRelativeLocation, TargetRelativeRotation, true, true, OverTime, false, EMoveComponentAction::Type::Move, LatentInfo);
}
FVector AProjectDemonCharacter::GetInputDirection()
{
	auto RightDirectRot = UKismetMathLibrary::MakeRotator(0, 0, GetControlRotation().Yaw);
	auto ForwardDirectRot = RightDirectRot;
	auto RightDirectVect = UKismetMathLibrary::GetRightVector(RightDirectRot);
	auto ForwardDirectVect = UKismetMathLibrary::GetForwardVector(ForwardDirectRot);
	auto InputDirection = RightDirectVect * MovingRightValue * 250 + ForwardDirectVect * MovingForwardValue * 250; 
	InputDirection.Normalize();
	return InputDirection;
}