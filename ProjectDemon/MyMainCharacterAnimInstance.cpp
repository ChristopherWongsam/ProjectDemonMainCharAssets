// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMainCharacterAnimInstance.h"
#include "ProjectDemonCharacter.h"
#include "DemonCharacter.h"
#include "Math/UnrealMathUtility.h"
#include"GameFramework//CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimSequence.h"
#include <Kismet/KismetSystemLibrary.h>

void UMyMainCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = TryGetPawnOwner();

	if (!Owner)
	{
		return;
	}
	if (Owner->IsA(ADemonCharacter::StaticClass()))
	{
		MyChar = Cast<ADemonCharacter>(Owner);
		if (MyChar)
		{
			UKismetSystemLibrary::PrintString(this, "Successful Instanciation of charcter!");
		}
		startBlink();
	}
}

void UMyMainCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (MyChar == nullptr)
	{
		MyChar = Cast<ADemonCharacter>(TryGetPawnOwner());
		return;
	}
	bIsMoving = MyChar->getSpeed() > 0.0;
	if (bIsMoving)
	{
		//UKismetSystemLibrary::PrintString(this, "Character is moving");
	}
	bIsFalling = MyChar->InAir();
	if (bIsFalling)
	{
		//UKismetSystemLibrary::PrintString(this, "Character is Falling");
	}
	bJumpButtonIsPressed = MyChar->getJumpButtonisPressed();
	bCharacterLanded = MyChar->getCharacterLanded();
	Speed = MyChar->getSpeed();
	bRunJumpCycle = MyChar->bCycleRunnigJumpMirror;
}

FVector UMyMainCharacterAnimInstance::CalculateLaunchVelocity(UAnimSequence* Sequence)
{

	return FVector(0,0,-1*GetWorld()->GetGravityZ() * Sequence->GetPlayLength()/1);
}

void UMyMainCharacterAnimInstance::AimOffset(const FVector& Location, const FRotator& Rotation,FName Bone)
{
	auto Rot = UKismetMathLibrary::FindLookAtRotation(MyChar->GetMesh()->GetSocketLocation("Spine1"),
		Location + UKismetMathLibrary::GreaterGreater_VectorRotator(FVector(2000, 0, 0), Rotation));

	FRotator TargetRotation;
	TargetRotation.Roll = UKismetMathLibrary::ClampAngle(-1 * Rot.Pitch, -165, 165);
	TargetRotation.Pitch = 0;
	TargetRotation.Yaw = UKismetMathLibrary::ClampAngle(Rot.Yaw - MyChar->GetActorRotation().Yaw, -165, 165);

	TargetRotation.Roll /= 3.0;
	TargetRotation.Pitch = 0;
	TargetRotation.Yaw /= 3.0;
	SpineRotation = UKismetMathLibrary::RInterpTo(SpineRotation, TargetRotation, GetWorld()->DeltaTimeSeconds, 20.0);
	if (Speed > 0)
	{
		MyChar->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	else
	{
		MyChar->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	return;
}

float UMyMainCharacterAnimInstance::PauseAnimMontage(float length, bool allowFunctionOverride)
{
	GetWorld()->GetTimerManager().ClearTimer(PauseAnimMontageTimer);
	auto CurrentActiveMontage = GetCurrentActiveMontage();
	CurrentPausedMontage = CurrentActiveMontage;
	//Montage_Pause(CurrentActiveMontage);
	float AnimPauseLength = allowFunctionOverride ? length : getPauseLength();
	montageOrigPlayRate = CurrentActiveMontage->RateScale;
	montageCurrPlayRate = montageOrigPlayRate;
	bEnableFloatDown = true;
	if (allowFunctionOverride)
	{
		GetWorld()->GetTimerManager().SetTimer(PauseAnimMontageTimer, this, &UMyMainCharacterAnimInstance::ResumePausedAnimMontage, length);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(PauseAnimMontageTimer, this, &UMyMainCharacterAnimInstance::ResumePausedAnimMontage, getPauseLength());
	}
	return AnimPauseLength;
}

void UMyMainCharacterAnimInstance::ResumePausedAnimMontage()
{
	GetWorld()->GetTimerManager().ClearTimer(PauseAnimMontageTimer);
	bEnableFloatDown = false;
	if (CurrentPausedMontage == GetCurrentActiveMontage())
	{
		Montage_SetPlayRate(CurrentPausedMontage, 1.0);
		Montage_Resume(CurrentPausedMontage);
	}
}

void UMyMainCharacterAnimInstance::SlowAnimMont(float rate)
{
	Montage_SetPlayRate(GetCurrentActiveMontage(),rate);
}
void UMyMainCharacterAnimInstance::ResumeMont(float rate)
{
	Montage_SetPlayRate(GetCurrentActiveMontage(), rate);
}



void UMyMainCharacterAnimInstance::startBlink()
{

	GetWorld()->GetTimerManager().SetTimer(blinkTimer, this, &UMyMainCharacterAnimInstance::Blink, 1.0);
}

void UMyMainCharacterAnimInstance::Blink()
{
	if (bEyesClosed)
	{
		float T = UKismetMathLibrary::RandomFloatInRange(5.0, 7.0);
		bEyesClosed = !bEyesClosed;
		SetMorphTarget("EyesClosedCurve", 0.0);
		GetWorld()->GetTimerManager().SetTimer(blinkTimer, this, &UMyMainCharacterAnimInstance::Blink, T);
		return;
	}
	float T = UKismetMathLibrary::RandomFloatInRange(0.2, 0.5);
	SetMorphTarget("EyesClosedCurve", 1.0);
	bEyesClosed = !bEyesClosed;
	GetWorld()->GetTimerManager().SetTimer(blinkTimer, this, &UMyMainCharacterAnimInstance::Blink, T);
	return;
}

float UMyMainCharacterAnimInstance::getAnimNotifyTime(UAnimSequence* animSequence, FString notifyNmae, FString notifyPrefix)
{
	if (!animSequence)
	{
		UKismetSystemLibrary::PrintString(this, "Anim sequence is null");
		return - 1.0;
	}
	for (FAnimNotifyEvent notifyEvent : animSequence->Notifies)
	{
		if (notifyEvent.GetNotifyEventName() == notifyPrefix + notifyNmae)
		{
			UKismetSystemLibrary::PrintString(this, "Notify Found");
			return notifyEvent.GetTime();
		}
	}
	UKismetSystemLibrary::PrintString(this, "Could not find notify");
	return -1.0;
}

