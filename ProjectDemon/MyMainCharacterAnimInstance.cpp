// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMainCharacterAnimInstance.h"
#include "ProjectDemonCharacter.h"
#include "DemonCharacter.h"
#include "Math/UnrealMathUtility.h"
#include"GameFramework//CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimSequence.h"

void UMyMainCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}
void UMyMainCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if (!GetAnimCharacter())
	{
		return;
	}
	if (GetAnimCharacter()->IsA(ADemonCharacter::StaticClass()))
	{
		MyChar = Cast<ADemonCharacter>(GetAnimCharacter());
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
		MyChar = Cast<ADemonCharacter>(GetAnimCharacter());
		return;
	}
	bIsMoving = MyChar->GetInputDirection().Size() > 0.0;
	bRunJumpCycle = MyChar->bCycleRunnigJumpMirror;
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

void UMyMainCharacterAnimInstance::startBlink()
{

	GetWorld()->GetTimerManager().SetTimer(blinkTimer, this, &UMyMainCharacterAnimInstance::Blink, 1.0);
}

void UMyMainCharacterAnimInstance::Blink()
{
	if (USkeletalMeshComponent* Component = GetOwningComponent())
	{
		if (Component->GetMorphTarget("EyesClosedCurve") == 0.0) 
		{
			return;
		}
	}
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

void UMyMainCharacterAnimInstance::setEnableMirror(bool EnableMirror)
{
	bEnableMirror = EnableMirror;
}

bool UMyMainCharacterAnimInstance::getEnableMirror()
{
	return bEnableMirror;
}


