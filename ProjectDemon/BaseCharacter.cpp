// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Private/InterpolateComponentToAction.h>
#include <Kismet/KismetMathLibrary.h>

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	defaultGravityScale = GetCharacterMovement()->GravityScale;
	defaultAirControl = GetCharacterMovement()->AirControl;
}
void ABaseCharacter::SpawnParticle(UParticleSystem* particleSystem,FVector Location,FRotator Rotation, FVector SpawnScale)
{
	if (!particleSystem)
	{
		Log("Particle System not valid");
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, particleSystem,Location, Rotation, SpawnScale, true);
}
void ABaseCharacter::Log(FString log, bool printToScreen)
{
	UKismetSystemLibrary::PrintString(this, log, printToScreen);
}
void ABaseCharacter::LogScreen(FString log, FLinearColor color)
{
	UKismetSystemLibrary::PrintString(this, log, true, true, color);
}
void ABaseCharacter::PrintLog(FString log)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *log);
}
void ABaseCharacter::Delay(float duration, FName funcName)
{
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, funcName);

	GetWorld()->GetTimerManager().SetTimer(
		timerHandler, // handle to cancel timer at a later time
		Delegate, // function to call on elapsed
		duration, // float delay until elapsed
		false); // looping?
}
void ABaseCharacter::CancelAllDelay()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}
void ABaseCharacter::setCanCancelAnimMontage(bool canCancelAnimMontage, UAnimMontage* montageToCancel)
{
	if (montageToCancel)
	{
		MontageToCancel = montageToCancel;
	}
	else
	{
		MontageToCancel = GetCurrentMontage();
	}
	bCanCancelAnimMontage = canCancelAnimMontage;
}
bool ABaseCharacter::getCanCancelAnimMontage()
{
	if (!MontageToCancel)
	{
		return false;
	}
	if (GetCurrentMontage() != MontageToCancel)
	{
		return false;
	}
	return bCanCancelAnimMontage;
}
void ABaseCharacter::ResetMovementComponentValues()
{
	GetCharacterMovement()->GravityScale = defaultGravityScale;
	GetCharacterMovement()->AirControl = defaultAirControl;
}
float ABaseCharacter::getMontageAnimNotifyTime(const UAnimMontage* Mont, FString notifyNmae, FString notifyPrefix)
{
	auto Time = -1.0;
	bool animFound = false;
	if (!Mont)
	{
		Log("Did not find AnimNotify: " + notifyNmae);
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

float ABaseCharacter::PlayMontage(UAnimMontage* Montage, FName Section, float rate, bool bEnaleLowerArmAnim)
{
	if (Montage)
	{
		bCanCancelAnimMontage = false;
		CanHitReact = true;
		auto T = GetMesh()->GetAnimInstance()->Montage_Play(Montage, rate);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(Section, Montage);
		auto time = Montage->GetSectionLength(Montage->GetSectionIndex(Section));
		time -= Montage->GetDefaultBlendOutTime();
		float Rate = Montage->RateScale;
		return time / Rate;
	}
	else
	{
		Log("Cannot play montage!");
		return -1.0;
	}
}
void ABaseCharacter::BindMontage(UAnimMontage* Montage, FName functionName)
{
	if (Montage)
	{
		FOnMontageEnded BlendOutDelegate;
		BlendOutDelegate.BindUFunction(this, functionName);
		GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, Montage);
	}
}

bool ABaseCharacter::SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, TArray<AActor*> ActorsToIgnore, int trace, FHitResult& HitResult, bool traceComplex, bool ignoreSelf)
{
	return UKismetSystemLibrary::SphereTraceSingle(this, StartPoint, EndPoint, sphereRadius, traceTypeQuery,
		traceComplex, ActorsToIgnore, (EDrawDebugTrace::Type)trace, HitResult, ignoreSelf);
}
bool ABaseCharacter::SphereTrace(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, FHitResult& HitResult, bool traceComplex, bool ignoreSelf)
{
	return UKismetSystemLibrary::SphereTraceSingle(this, StartPoint, EndPoint, sphereRadius, traceTypeQuery,
		traceComplex, actorsToIgnore, (EDrawDebugTrace::Type)trace, HitResult, ignoreSelf);
}
bool ABaseCharacter::SphereTraceMulti(FVector StartPoint, FVector EndPoint, float sphereRadius, ETraceTypeQuery traceTypeQuery, int trace, TArray<FHitResult>& HitResults, bool traceComplex, bool ignoreSelf)
{
	return UKismetSystemLibrary::SphereTraceMulti(this, StartPoint, EndPoint, sphereRadius, traceTypeQuery,
		traceComplex, actorsToIgnore, (EDrawDebugTrace::Type)trace, HitResults, ignoreSelf);
}
void ABaseCharacter::MoveCharacterToRotationAndLocationIninterval(FVector TargetLocation, FRotator TargetRotation, float OverTime)
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
}

void ABaseCharacter::cancelMoveCharacterToRotationAndLocationIninterval()
{
	FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
}
ETurnState ABaseCharacter::TurnInPlace(FRotator CameraRotation)
{
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(CameraRotation, GetActorRotation());

	if (DeltaRotator.Yaw > 90.0 && DeltaRotator.Yaw <= 135)
	{
		//Play right
		return ETurnState::ETS_Right;
	}
	else if(DeltaRotator.Yaw < -90.0 && DeltaRotator.Yaw >= -135.0)
	{
		//Play left
		return ETurnState::ETS_Left;
	}
	else if (DeltaRotator.Yaw < -135.0 && DeltaRotator.Yaw >= -180)
	{
		//Play Left half
		return ETurnState::ETS_LeftHalf;
	}
	else if (DeltaRotator.Yaw > 135 && DeltaRotator.Yaw <= 180)
	{
		//Play Right Half
		return ETurnState::ETS_RightHalf;
	}
	else
	{
		return ETurnState::ETS_None;
	}
}
float ABaseCharacter::newValueFromChange(float currentValue, float newValue)
{
	if (newValue == 0.0)
	{
		return currentValue;
	}
	if (TimeInterpValue != newValue)
	{
		if (TimeInterpValue == 0.0)
		{
			TimeInterpValue = newValue;
		}
		auto ratio = currentValue / TimeInterpValue;
		auto NewCurrentTime = ratio * newValue;
		TimeInterpValue = newValue;
		return NewCurrentTime;
	}
	return currentValue;
}
float ABaseCharacter::InterpValueTime(float currentValue, float targetValue, float DeltaTime)
{
	float newVal = newValueFromChange(currentValue, targetValue);
	if (newVal == targetValue)
	{
		return newVal;
	}
	else if (currentValue < targetValue)
	{
		newVal += DeltaTime;
		if (currentValue > targetValue)
		{
			newVal = targetValue;
		}
		return newVal;
	}
	else
	{
		newVal -= DeltaTime;
		if (newVal < targetValue)
		{
			newVal = targetValue;
		}
		return newVal;
	}
}

void ABaseCharacter::setEnableHitbox(bool enableHitbox)
{
	bEnableHitBox = enableHitbox;
	if (!bEnableHitBox)
	{
		actorsHit.Empty();
	}
}
void ABaseCharacter::setEnableLimbHitbox(bool enableHitbox, FName LimbAttackSocketName)
{
	bEnableHitBox = enableHitbox;
	AttackSocketName = LimbAttackSocketName;
	if (!bEnableLimbHitBox)
	{
		actorsHit.Empty();
	}
}
void ABaseCharacter::RestartHitbox()
{
	actorsHit.Empty();
	setEnableHitbox(false);
}
float ABaseCharacter::getSpeed()
{
	return GetCharacterMovement()->Velocity.Size();
}
bool ABaseCharacter::InAir() const
{
	return GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling || GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying;
}
