// Fill out your copyright notice in the Description page of Project Settings.


#include "DemonCharacter.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Private/InterpolateComponentToAction.h>
#include "MyMainCharacterAnimInstance.h"
#include "Enemy.h"



void ADemonCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainCharacterAnimInstance = Cast<UMyMainCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (MainCharacterAnimInstance)
	{
		Log("Unable to cast MainCharacterAnimInstance");
	}
}
void ADemonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Mantle(DeltaTime);
	SoftLock(DeltaTime);
	//DrawInput(DeltaTime);
}
void ADemonCharacter::DrawInput(float DeltaTime)
{
	auto start = GetActorLocation();
	auto end = GetActorLocation() + GetInputDirection() * 50;
	auto lineThickness = 50.0;
	FHitResult Result;
	UKismetSystemLibrary::DrawDebugArrow(this, start, end, lineThickness, FLinearColor::Yellow);
}

void ADemonCharacter::Mantle(float DeltaTime)
{
	if (!getMovementInputReceived())
	{
		return;
	}
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		return;
	}
	TArray<AActor*> ActorsToIgnore;
	FVector StartPoint = GetActorLocation();
	//StartPoint.Z -= 10;
	FVector EndPoint = StartPoint + GetActorForwardVector() * 50;
	FHitResult Result;
	bool didHit = UKismetSystemLibrary::SphereTraceSingle(this, StartPoint, EndPoint, 30.0, ETraceTypeQuery::TraceTypeQuery1, 
		false, ActorsToIgnore, EDrawDebugTrace::None, Result,true);
	if (didHit)
	{
		FHitResult WallHit = Result;
		StartPoint = Result.ImpactPoint + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
		StartPoint.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		EndPoint = StartPoint;
		EndPoint.Z -= GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

		FHitResult FloorHit;

		didHit = SphereTrace(StartPoint, EndPoint, 30, ETraceTypeQuery::TraceTypeQuery1, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, FloorHit);

		if (didHit)
		{
			if (FloorHit.ImpactPoint.Z < GetActorLocation().Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight() *.25 && 
				FloorHit.ImpactPoint.Z > GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * .25)
			{
				Log("Can mantle");
			}
			else
			{
				return;
			}
			if (!MantleMontage)
			{
				return;
			}
			GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
			GetMovementComponent()->StopMovementImmediately();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			auto floatInTime = MantleMontage->GetDefaultBlendInTime();
			auto newLoc = WallHit.ImpactPoint;
			newLoc.Z += mantleZOffsset;

			auto newRot = WallHit.Normal.Rotation().Yaw + 180;
			auto playerRot = GetActorRotation();
			//playerRot.Yaw = newRot;

			GetCameraBoom()->bDoCollisionTest = false;

			FOnMontageEnded BlendOutDelegate;
			

			float delay = PlayMontage(MantleMontage);
			BlendOutDelegate.BindUObject(this, &ADemonCharacter::MantleEnd);
			GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, MantleMontage);

			Log("Mantle length is :" + FString::SanitizeFloat(delay));
			MoveCharacterToRotationAndLocationIninterval(newLoc, playerRot, floatInTime);
		}
	}
}
void ADemonCharacter::MantleEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	Log("Mantle end");
	auto newLoc = GetActorLocation();
	newLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	auto playerRot = GetActorRotation();
	GetMovementComponent()->StopMovementImmediately();

	auto StartPoint = GetActorLocation();
	StartPoint.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	auto EndPoint = StartPoint;
	EndPoint.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10;
	FHitResult Result;
	bool didHit = SphereTrace(StartPoint, EndPoint, 30, ETraceTypeQuery::TraceTypeQuery1, EDrawDebugTrace::None, Result);
	GetCameraBoom()->bDoCollisionTest = true;
	if (didHit)
	{
		Log("Floor ground hit");
		newLoc = Result.ImpactPoint;
		newLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		playerRot = GetActorRotation();
		GetMovementComponent()->StopMovementImmediately();
		MoveCharacterToRotationAndLocationIninterval(newLoc, playerRot, 0.2);
		ResetCollision();
		return;
	}
	
	MoveCharacterToRotationAndLocationIninterval(newLoc, playerRot, 0.2);
	ResetCollision();
}
void ADemonCharacter::freeflowEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	Log("Freeflow end");
	bCanGoToNextFreeflow = true;
	ResetCollision();
}

void ADemonCharacter::StartFreeflow(bool enableDebug)
{
	if (!bCanGoToNextFreeflow)
	{
		return;
	}
	auto inputDirection = GetInputDirection();
	

	TArray<AActor*> outActors = GetActorsFromSphere();
	Log("Num of actors in sphere: " + FString::FromInt(outActors.Num()));

	FVector bestVect = FVector::ZeroVector;
	FVector loc;

	if (inputDirection.Size() == 0.0)
	{
		inputDirection = GetActorForwardVector();
	}
	for (AActor* actor : outActors)
	{
		if (AEnemy* enemy = Cast<AEnemy>(actor))
		{
			ACharacter* freeflowChar = Cast<ACharacter>(enemy);
			Log("Found tag");
			auto vect = actor->GetActorLocation() - GetActorLocation();
			vect.Normalize();
			auto start = GetActorLocation();
			auto end = start + vect * 150;
			auto lineThickness = 100;
			if (enableDebug)
			{
				UKismetSystemLibrary::DrawDebugArrow(this, start, end, lineThickness, FLinearColor::Red, 5.0, 2.0);
			}
			
			start = actor->GetActorLocation();
			vect *= -1;

			end = start + vect * (freeflowChar->GetCapsuleComponent()->GetScaledCapsuleRadius() + freeflowDisFromTarget);
			vect *= -1;
			
			if (FVector::DotProduct(bestVect, inputDirection) < FVector::DotProduct(vect, inputDirection) &&
				FVector::DotProduct(vect, inputDirection) > 0.5)
			{

				bestVect = vect;
				loc = end;
			}

			if (enableDebug)
			{
				UKismetSystemLibrary::DrawDebugArrow(this, start, end, lineThickness, FLinearColor::Green, 5.0, 2.0);
			}
			
		}
	}

	if (bestVect != FVector::ZeroVector)
	{
		Log("Found Best Vect");
		auto start = GetActorLocation();
		auto end = start + bestVect * 150;
		auto lineThickness = 100;
		if (enableDebug)
		{
			UKismetSystemLibrary::DrawDebugArrow(this, start, end, lineThickness, FLinearColor::Blue, 5.0, 2.0);
		}
		
	}
	else
	{
		return;
	}

	if (FreeflowAttackMontageArray.Num() > 0 )
	{
		auto randInt = FMath::Rand() % FreeflowAttackMontageArray.Num();
		if (FreeflowAttackMontageArray.IsValidIndex(randInt))
		{
			GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
			GetMovementComponent()->StopMovementImmediately();
			GetCameraBoom()->bDoCollisionTest = false;
			auto mont = FreeflowAttackMontageArray[randInt];
			auto moveToTime = getMontageAnimNotifyTime(mont, "FreeflowEndNotify");

			auto desiredTime = 0.5;
			auto rate = moveToTime / desiredTime;
			PlayMontage(mont,"Default");
			MainCharacterAnimInstance->Montage_SetPlayRate(mont, rate);
			FOnMontageEnded BlendOutDelegate;
			BlendOutDelegate.BindUObject(this, &ADemonCharacter::freeflowEnd);
			GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, mont);

			

			auto newRot = FRotator::ZeroRotator;
			newRot.Yaw = bestVect.Rotation().Yaw;
			SetActorRotation(newRot);
			bCanGoToNextFreeflow = false;

			auto T = desiredTime;
			if (enableDebug)
			{
				UKismetSystemLibrary::DrawDebugSphere(this, loc, 30, 12, FLinearColor::Blue, 2.0);
			}
			MoveCharacterToRotationAndLocationIninterval(loc, GetActorRotation(), T);
		}
	}
}
void ADemonCharacter::canGoToNextfreeflow()
{
	Log("Character can proceed to new free flow");
	bCanGoToNextFreeflow = true;
	GetMovementComponent()->StopMovementImmediately();
	if (GetMesh()->GetAnimInstance()->GetCurrentActiveMontage())
	{
		auto currentMont = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
		GetMesh()->GetAnimInstance()->Montage_SetPlayRate(currentMont);
	}
	
}
void ADemonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Jumping
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &ADemonCharacter::LeftMouseClick);
		EnhancedInputComponent->BindAction(LeftShiftAction, ETriggerEvent::Started, this, &ADemonCharacter::ShiftClick);
		EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &ADemonCharacter::LeftMouseClick);
		EnhancedInputComponent->BindAction(LeftCtrltAction, ETriggerEvent::Started, this, &ADemonCharacter::LeftCTRLClick);
		EnhancedInputComponent->BindAction(LeftCtrltAction, ETriggerEvent::Completed, this, &ADemonCharacter::LeftCTRLClickEnd);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
	
}
void ADemonCharacter::LeftCTRLClick()
{
	Log("left ctrl click");
	bLeftCtrlButtonIsHeld = true;
	//StartFreeflow();
}
void ADemonCharacter::LeftCTRLClickEnd()
{
	Log("left ctrl end");
	bLeftCtrlButtonIsHeld = false;
	//StartFreeflow();
}
void ADemonCharacter::LeftMouseClick()
{
	Log("left mouse click");
	if (bLeftCtrlButtonIsHeld)
	{
		StartFreeflow();
	}
	else
	{
		PlayerAttack();
	}
}
void ADemonCharacter::ShiftClick()
{
	Log("Shift click");
	PlayerDodge();
}
void ADemonCharacter::PlayerDodgeEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	Log("Player dodge ended");
	if (bInterrupted)
	{
		playerCanDodge = true;
	}
}
void ADemonCharacter::NextDodge()
{
	playerCanDodge = true;
}
void ADemonCharacter:: PlayerAttack()
{
	Log("Player Attack commence");
	if (!playerCanDodge)
	{
		Log("Player cannot attack in dodge transition");
		return;
	}
	if (!playerCanAttck)
	{
		Log("Player cannot attack.");
		return;
	}
	TArray<UAnimMontage*> upperAttckMontage;
	AttackMontageMap.GenerateKeyArray(upperAttckMontage);
	if (upperAttckMontage.Num() == 0)
	{
		Log("No attack montage.");
		return;
	}
	if (!upperAttckMontage.IsValidIndex(currentAttackIndex))
	{
		Log("Attack index is resetting.");
		currentAttackIndex = 0;
	}
	
	auto attackMontage = upperAttckMontage[currentAttackIndex];
	Log("Player will attack");
	PlayMontage(attackMontage);
	FOnMontageEnded BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ADemonCharacter::PlayerAttackEnd);
	currentAttackIndex++;
	playerCanAttck = false;
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, attackMontage);

	if (playerEnemy)
	{
		auto rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), playerEnemy->GetActorLocation());
		auto newRot = GetActorRotation();
		newRot.Yaw = rot.Yaw;
		MoveCharacterToRotationAndLocationIninterval(GetActorLocation(), newRot, attackMontage->GetDefaultBlendInTime());
	}
}
void ADemonCharacter::Move(const FInputActionValue& Value)
{
	float inputVal = Value.GetMagnitudeSq();
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovingForwardValue = MovementVector.Y;
	MovingRightValue = MovementVector.X;

	//Super::Move(Value);
	Log("Input value magnitude: " + FString::SanitizeFloat(inputVal));
	Value.GetValueType();

	if (Value.GetValueType() == EInputActionValueType::Axis2D)
	{
		Super::Move(Value);
	}
	else
	{
		Log("Using Controller");
		if (inputVal > 5.0)
		{
			Super::Move(Value);
		}
	}
	
	
	if (GetIsDodgeAnimationPlaying() && playerCanDodge)
	{
		auto currMont = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
		if (currMont)
		{
			GetMesh()->GetAnimInstance()->Montage_Stop(currMont->GetDefaultBlendOutTime(), currMont);
		}
	}
}
void ADemonCharacter::SoftLock(float DeltaTime)
{
	int radius = 200;
	TArray<AActor*> actors = GetActorsFromSphere(radius);
	while (radius < 1200 && actors.Num() < 2)
	{
		actors = GetActorsFromSphere(radius);
		radius += 200;
	}
	if (actors.Num() == 0)
	{
		return;
	}
	float bestAngle = -1.0;
	AEnemy* selectedEnemy = nullptr;
	FVector vectToComp = GetActorForwardVector();
	if (GetInputDirection().Size() > 0.0)
	{
		vectToComp = GetInputDirection();
	}
	for (AActor* actor : actors)
	{
		if (AEnemy* enemy = Cast<AEnemy>(actor))
		{
			FVector enemyToCharVect = enemy->GetActorLocation() - GetActorLocation();
			enemyToCharVect.Normalize();
			

			float dotProduct = FVector::DotProduct(enemyToCharVect, vectToComp);
			//Log("The dot product is: " + FString::SanitizeFloat(dotProduct));
			if (dotProduct > bestAngle)
			{
				bestAngle = dotProduct;
				selectedEnemy = enemy;
			}
		}
	}
	if (!selectedEnemy && playerEnemy)
	{
		playerEnemy->enableOutline(false);
		return;
	}
	else if(!selectedEnemy)
	{
		//Do nothing
	}
	else if (!playerEnemy)
	{
		selectedEnemy->enableOutline(true);
		playerEnemy = selectedEnemy;
	}
	else if (selectedEnemy != playerEnemy)
	{
		selectedEnemy->enableOutline(true);
		playerEnemy->enableOutline(false);
		playerEnemy = selectedEnemy;
	}
}
void ADemonCharacter::PlayerDodge()
{
	Log("Player Dodge commence");
	if (!playerCanDodge)
	{
		return;
	}
	
	if (DodgeMontageArray.Num() == 0)
	{
		Log("No dodge attacks");
		return;
	}
	int dodgeIndex = FMath::Rand() % DodgeMontageArray.Num();
	if (!DodgeMontageArray.IsValidIndex(dodgeIndex))
	{
		Log("Not valid random index?");
		return;
	}
	auto DodgeMontage = DodgeMontageArray[dodgeIndex];
	PlayMontage(DodgeMontage);
	playerCanDodge = false;
	FOnMontageEnded BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ADemonCharacter::PlayerDodgeEnd);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, DodgeMontage);
}
FVector ADemonCharacter::GetInputDirection()
{
	auto followCamera = GetFollowCamera();
	auto forwardVect = followCamera->GetForwardVector() * MovingForwardValue;
	auto rightVect = followCamera->GetRightVector() * MovingRightValue;
	auto InputDirection = forwardVect + rightVect;
	InputDirection.Normalize();

	return InputDirection;
}
void ADemonCharacter::PlayerAttackEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	Log("Player Attack End");
	if (!bInterrupted)
	{
		currentAttackIndex = 0;
	}
	else
	{
		playerCanAttck = true;
	}
}
void ADemonCharacter::NextAttack()
{
	Log("Player Can go to next attack");
	playerCanAttck = true;
}
bool ADemonCharacter::GetIsAttackAnimationPlaying()
{
	TArray<UAnimMontage*> upperAttckMontage;
	AttackMontageMap.GenerateKeyArray(upperAttckMontage);
	if (upperAttckMontage.Num() == 0)
	{
		return false;
	}
	for (UAnimMontage* mont : upperAttckMontage)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsActive(mont))
		{
			return true;
		}
	}
	return false;
}
bool ADemonCharacter::GetIsDodgeAnimationPlaying()
{
	if (DodgeMontageArray.Num() == 0)
	{
		return false;
	}
	for (UAnimMontage* mont : DodgeMontageArray)
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsActive(mont))
		{
			return true;
		}
	}
	return false;
}
TArray<AActor*> ADemonCharacter::GetActorsFromSphere(float radius,bool enableDebug)
{
	TArray<FHitResult> Results;

	// Set what actors to seek out from it's collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// Ignore any specific actors
	TArray<AActor*> ignoreActors;
	// Ignore self or remove this line to not ignore any
	ignoreActors.Init(this, 1);

	// Array of actors that are inside the radius of the sphere
	TArray<AActor*> outActors;

	// Total radius of the sphere
	
	// Sphere's spawn loccation within the world
	FVector sphereSpawnLocation = GetActorLocation();
	// Class that the sphere should hit against and include in the outActors array (Can be null)
	UClass* seekClass = AEnemy::StaticClass(); // NULL;

	if (enableDebug)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), radius);
	}
	

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), sphereSpawnLocation, radius, traceObjectTypes, seekClass, ignoreActors, outActors);

	return outActors;
}