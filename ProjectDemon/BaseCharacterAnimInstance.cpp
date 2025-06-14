// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/Character.h"
#include <Kismet/KismetSystemLibrary.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterAnimInstance.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	APawn* Owner = TryGetPawnOwner();
	if (!Owner)
	{
		return;
	}
	if (Owner->IsA(ABaseCharacter::StaticClass()))
	{
		AnimCharacter = Cast<ABaseCharacter>(Owner);
	}
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (AnimCharacter == nullptr)
	{
		AnimCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
		return;
	}
	bIsFalling = AnimCharacter->InAir();
	Speed = AnimCharacter->getSpeed();
	Direction = CalculateDirection(AnimCharacter->GetVelocity(), AnimCharacter->GetActorRotation());
	UpdateMontageRootMotion(DeltaSeconds);
	
}
FVector UBaseCharacterAnimInstance::getRootMotionDataArray(UAnimMontage* Montage)
{
	if (!Montage)
	{
		return FVector::ZeroVector;
	}
	Montage_GetPosition(Montage);
	for (auto section : Montage->CompositeSections)
	{

		FVector BoneLocation;
		section.GetLinkedSequence();
		if (const UAnimSequence* animSequence = Cast<UAnimSequence>(section.GetLinkedSequence()))
		{
			int size = animSequence->GetNumberOfSampledKeys();
			for (size_t frame = 0; frame < size;frame++)
			{
				FTransform OutAtom;
				auto T = animSequence->GetTimeAtFrame(frame);
				FSkeletonPoseBoneIndex BoneIndex(0);
				animSequence->GetBoneTransform(OutAtom, BoneIndex, T, true);
				Log("Root motion index is: " + OutAtom.GetLocation().ToString());
			}
		}
		else
		{
			Log("Invalid animation");
		}
	}
	return FVector::ZeroVector;
}
FVector UBaseCharacterAnimInstance::getRootMotionDataAtFrame(UAnimMontage* Montage, int frame)
{
	if (!Montage)
	{
		return FVector::ZeroVector;
	}
	Montage_GetPosition(Montage);
	for (auto section : Montage->CompositeSections)
	{
		Montage_GetCurrentSection();
		if (Montage_GetCurrentSection() == section.SectionName)
		{
			if (const UAnimSequence* animSequence = Cast<UAnimSequence>(section.GetLinkedSequence()))
			{
				FTransform OutAtom;
				auto T = animSequence->GetTimeAtFrame(frame);
				FSkeletonPoseBoneIndex BoneIndex(0);
				animSequence->GetBoneTransform(OutAtom, BoneIndex, T, true);
				Log("Root motion index is: " + OutAtom.GetLocation().ToString());	
			}

		}
		else
		{
			//Log("Invalid animation");
		}
	}
	return FVector::ZeroVector;
}
const UAnimSequence* UBaseCharacterAnimInstance::GetCurrentAnimSequenceFromMontage()
{
	UAnimMontage* Montage = GetCurrentActiveMontage();
	if (!GetCurrentActiveMontage())
	{
		return nullptr;
	}
	Montage_GetPosition(Montage);
	for (auto section : Montage->CompositeSections)
	{
		Montage_GetCurrentSection();
		if (Montage_GetCurrentSection() == section.SectionName)
		{
			if (const UAnimSequence* animSequence = Cast<UAnimSequence>(section.GetLinkedSequence()))
			{
				
			}

		}
		else
		{
			//Log("Invalid animation");
		}
	}
	return nullptr;
}
FVector UBaseCharacterAnimInstance::getRootMotionDataFromActiveMontage(float time)
{
	const UAnimMontage* Montage = GetCurrentActiveMontage();
	if (!Montage)
	{
		return FVector::ZeroVector;
	}
	float totalTime = 0.0;
	for (FCompositeSection section : Montage->CompositeSections)
	{
		if (Montage_GetCurrentSection() == section.SectionName)
		{
			if (const UAnimSequence* animSequence = Cast<UAnimSequence>(section.GetLinkedSequence()))
			{
				FTransform OutAtom;
				if (time > 0.0)
				{
					auto T = time - totalTime;
					if (T > 0.0)
					{
						FSkeletonPoseBoneIndex BoneIndex(0);
						animSequence->GetBoneTransform(OutAtom, BoneIndex, T, true);
						return OutAtom.GetLocation();
					}
				}
				totalTime +=  animSequence->GetPlayLength();
			}
		}
		
	}
	return FVector::ZeroVector;
}
FVector UBaseCharacterAnimInstance::getRootMotionDataFromActiveMontage()
{
	const UAnimMontage* Montage = RootMotionMontage;
	return getRootMotionDataFromActiveMontage(Montage_GetPosition(Montage));
}
void UBaseCharacterAnimInstance::bindMontageRootMotionModifier(UAnimMontage* Montage, float rootMotionScale)
{
	bindMontageRootMotionModifier(Montage, FVector(rootMotionScale));
}

void UBaseCharacterAnimInstance::bindMontageRootMotionModifier(UAnimMontage* Montage, FVector vectorScale)
{
	// Take into consideration of forward vector;
	if (!Montage)
	{
		Log("Cannot bind. Montage is not valid.");
		return;
	}

	rootMotionVectorScale = vectorScale;
	AnimCharOriginalLocation = AnimCharacter->GetActorLocation();
	AnimCharForwardVector = AnimCharacter->GetActorForwardVector();
	AnimCharUpVector = AnimCharacter->GetActorUpVector();
	AnimCharRightVector = AnimCharacter->GetActorRightVector();
	RootMotionMontage = Montage;
	bEnableRootMotionModifier = true;
}

void UBaseCharacterAnimInstance::UpdateMontageRootMotion(float DeltaTime)
{
	if (bEnableRootMotionModifier)
	{
		if (Montage_IsActive(RootMotionMontage))
		{
			FVector Offset = getRootMotionDataFromActiveMontage();
			FVector NewCharacterLocation = AnimCharOriginalLocation +
				Offset.Y * AnimCharForwardVector * rootMotionVectorScale.Y +
				Offset.Z * AnimCharUpVector * rootMotionVectorScale.Z +
				Offset.X * AnimCharRightVector * rootMotionVectorScale.X;
			FVector Velocity = (NewCharacterLocation - AnimCharacter->GetActorLocation()) / DeltaTime;
			AnimCharacter->GetCharacterMovement()->Velocity = Velocity;
			//AnimCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{
			//AnimCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
			bEnableRootMotionModifier = false;
			AnimCharacter->GetCharacterMovement()->StopMovementImmediately();
		}
	}
}

float UBaseCharacterAnimInstance::PauseAnimMontage(float length, float playRate, bool allowFunctionOverride)
{
	GetWorld()->GetTimerManager().ClearTimer(PauseAnimMontageTimer);
	auto CurrentActiveMontage = GetCurrentActiveMontage();
	CurrentPausedMontage = CurrentActiveMontage;

	Montage_SetPlayRate(CurrentActiveMontage, playRate);

	GetWorld()->GetTimerManager().SetTimer(PauseAnimMontageTimer, this, &UBaseCharacterAnimInstance::ResumePausedAnimMontage,
		allowFunctionOverride ? length : getPauseLength());

	return allowFunctionOverride ? length : getPauseLength();
}

void UBaseCharacterAnimInstance::ResumePausedAnimMontage()
{
	GetWorld()->GetTimerManager().ClearTimer(PauseAnimMontageTimer);
	bEnableFloatDown = false;
	if (CurrentPausedMontage == GetCurrentActiveMontage())
	{
		Montage_SetPlayRate(CurrentPausedMontage, 1.0);

	}
}

void UBaseCharacterAnimInstance::SlowAnimMont(float rate)
{
	Montage_SetPlayRate(GetCurrentActiveMontage(), rate);
}
void UBaseCharacterAnimInstance::ResumeMont(float rate)
{
	Montage_SetPlayRate(GetCurrentActiveMontage(), rate);
}

float UBaseCharacterAnimInstance::getAnimNotifyTime(UAnimSequence* animSequence, FString notifyNmae, FString notifyPrefix)
{
	if (!animSequence)
	{
		UKismetSystemLibrary::PrintString(this, "Anim sequence is null");
		return -1.0;
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

void UBaseCharacterAnimInstance::Log(FString log, bool printToScreen)
{
	UKismetSystemLibrary::PrintString(this, log, printToScreen);
}