// Fill out your copyright notice in the Description page of Project Settings.


#include "DemonCharacter.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Private/InterpolateComponentToAction.h>
#include "MyMainCharacterAnimInstance.h"
#include "UObject/UObjectGlobals.h"
#include "Enemy.h"
#include "C:/UE_5.4/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"



void ADemonCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	for (int index : MaterialIndexHide)
	{
		GetMesh()->ShowMaterialSection(index, 0,false,0);
	}
}

void ADemonCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainCharacterAnimInstance = Cast<UMyMainCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (!MainCharacterAnimInstance)
	{
		Log("Unable to cast MainCharacterAnimInstance");
	}
	orignalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//getRootMotionData(DodgeMontageArray[0]);
}
void ADemonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Mantle(DeltaTime);
	SoftLock(DeltaTime);
	//StartHitbox(DeltaTime,true,false);
	UpdateHitbox(DeltaTime);
	Swing(DeltaTime,true);
	UpdateBoost(DeltaTime);
	UpdateWallclimb(DeltaTime);
	UpdateMovementRotation(DeltaTime);
	UpdatePlayerAttack(DeltaTime);
	UpdateCamera(DeltaTime);
}
void ADemonCharacter::DrawInput(float DeltaTime)
{
	auto start = GetActorLocation();
	auto end = GetActorLocation() + GetInputDirection() * 50;
	auto lineThickness = 50.0;
	FHitResult Result;
	UKismetSystemLibrary::DrawDebugArrow(this, start, end, lineThickness, FLinearColor::Yellow);
}

void ADemonCharacter::UpdateSpeed(float DeltaTime)
{
	
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


	TArray<UAnimMontage*> FreeflowAttackMontageArray;
	FreeflowAttackMontageMap.GenerateKeyArray(FreeflowAttackMontageArray);

	if (FreeflowAttackMontageArray.Num() > 0 )
	{
		auto randInt = FMath::Rand() % FreeflowAttackMontageArray.Num();
		if (FreeflowAttackMontageArray.IsValidIndex(randInt))
		{
			//Setting up character movement
			//GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
			GetMovementComponent()->StopMovementImmediately();
			//Modifying camera collision
			GetCameraBoom()->bDoCollisionTest = false;

			//Getting Anim notify time
			auto mont = FreeflowAttackMontageArray[randInt];
			auto moveToTime = getMontageAnimNotifyTime(mont, "FreeflowEndNotify");

			//Setting the time to move to object
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
void ADemonCharacter::UpdatePlayerAttack(float DeltaTime)
{
	if (!MainCharacterAnimInstance->Montage_IsActive(AttackRushMontage))
	{
		return;
	}
	if (playerEnemy != nullptr)
	{
		float distanceFromEnemy = getDistanceFromCharacter(playerEnemy);
		if(attackRushMinimumDistance >= distanceFromEnemy)
		{
			MainCharacterAnimInstance->Montage_Stop(0.2, AttackRushMontage);
			PlayerAttack();
		}
	}
}
float ADemonCharacter::getDistanceFromCharacter(ACharacter* character)
{
	if (character)
	{
		float capsuleRadius = character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float totalDistance = FVector::Dist(GetActorLocation(), character->GetActorLocation());

		return  totalDistance - capsuleRadius;
	}
	Log("Not valid character");
	return -1.0;
}
void ADemonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Jumping
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ADemonCharacter::onMoveStarted);
		EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &ADemonCharacter::LeftMouseClick);
		EnhancedInputComponent->BindAction(LeftShiftAction, ETriggerEvent::Started, this, &ADemonCharacter::ShiftClick);
		EnhancedInputComponent->BindAction(LeftCtrltAction, ETriggerEvent::Started, this, &ADemonCharacter::LeftCTRLClick);
		EnhancedInputComponent->BindAction(LeftCtrltAction, ETriggerEvent::Completed, this, &ADemonCharacter::LeftCTRLClickEnd);
		EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Started, this, &ADemonCharacter::RightMouseClick);
		EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Completed, this, &ADemonCharacter::RightMouseClickEnd);

		EnhancedInputComponent->BindAction(EPressAction, ETriggerEvent::Completed, this, &ADemonCharacter::EKeyActionPress);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADemonCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADemonCharacter::StopJumping);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
	
}
void ADemonCharacter::StartJump()
{
	
	if (GetCurrentMontage())
	{
		if (GetCurrentMontage() == JumpLandMontage)
		{
			MainCharacterAnimInstance->Montage_Stop(0.0, JumpLandMontage);
		}
	}
	bJumpButtonIsPressed = true;
	if (JumpStartMontage)
	{
		if (getSpeed() > 0.0)
		{
			PlayMontage(JumpStartMontage,"JumpRun");
		}
		else
		{
			PlayMontage(JumpStartMontage);
		}
		
		float time = getMontageAnimNotifyTime(JumpStartMontage, "JumpLaunch");
		if (time < 0.0)
		{
			Super::Jump();
		}
	}
	else
	{
		Super::Jump();
	}
}
void ADemonCharacter::StopJumping()
{
	bJumpButtonIsPressed = false;
	ToggleBoost(false,false);
	Super::StopJumping();
}
void ADemonCharacter::LeftCTRLClick()
{
	Log("left ctrl click");
	bLeftCtrlButtonIsHeld = true;
}
void ADemonCharacter::LeftCTRLClickEnd()
{
	Log("left ctrl end");
	bLeftCtrlButtonIsHeld = false;
}
void ADemonCharacter::LeftMouseClick()
{
	Log("left mouse click");
	if (bLeftCtrlButtonIsHeld)
	{
		//StartFreeflow();
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
void ADemonCharacter::RightMouseClick()
{
	Log("Right mouse click");
	//To Swing or not to swing?
	//StartSwing();
}
void ADemonCharacter::RightMouseClickEnd()
{
	Log("Right mouse click");
}

void ADemonCharacter::ToggleBoost(bool reset, bool activate)
{
	if (reset)
	{
		BoostTime = BoostMaxTime;
	}
	bIsGliding = activate;
	float newAirontrol = defaultAirControl;
	if (bIsGliding)
	{
		newAirontrol = 5.0;
	}
	GetCharacterMovement()->AirControl = newAirontrol;
}
void ADemonCharacter::UpdateBoost(float DeltaTime)
{
	if (!bIsGliding)
	{
		return;
	}
	BoostTime -= DeltaTime;
	LaunchCharacter(FVector(0, 0, 120), false, true);
	if (BoostTime <= 0.0)
	{
		ToggleBoost(false, false);
	}
}
void ADemonCharacter::Swing(float DeltaTime,bool enableDebug)
{
	if (!bEnableSwing)
	{
		return;
	}
	FVector Vn = OrbitPoint - GetActorLocation();
	Vn.Normalize();
	FVector Vt = FVector::CrossProduct(GetActorRightVector(), Vn);
	Vt.Normalize();

	FVector PlayerToLocVect = GetActorLocation() - OrbitPoint;
	PlayerToLocVect.Normalize();

	FVector projectedVector = FVector::ZeroVector;

	Vt.Normalize();
	


	auto DirVect = OrbitPoint - GetActorLocation();
	auto DistPlayToOrbit = FVector::Dist(OrbitPoint, GetActorLocation());
	DirVect.Normalize();
	auto xyVelocity = Vt;


	auto orbitRadius = 200;
	auto speed = SwingSpeed;
	auto Chord = speed * DeltaTime;

	auto SinHalfAng = Chord / (2.0 * orbitRadius);
	auto CosHalfAngle = FMath::Sqrt(1 - SinHalfAng * SinHalfAng);
	auto upVectorStrength = DirVect * SinHalfAng * Chord;
	auto ForwardVelocity = xyVelocity * CosHalfAngle * Chord;
	auto playerVelocityDir = ForwardVelocity + upVectorStrength;

	playerVelocityDir.Normalize();

	GetCharacterMovement()->Velocity = playerVelocityDir * speed;

	if (enableDebug)
	{
		auto UpVect = Vn;
		auto RightVect = GetActorRightVector();

		DrawDebugSphere(GetWorld(), OrbitPoint, 50, 32, FColor::Blue);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + UpVect * 200, 50, FLinearColor::White, 0.0, 15);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + RightVect * 200, 50, FLinearColor::White, 0.0, 15);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + playerVelocityDir * 250, 50, FLinearColor::Gray, 0.0, 15);
	}
}
void ADemonCharacter::StartSwing()
{
	if (InAir())
	{
		EnableSwing();
		return;
	}
	Log("Starting swing");
	float testHeight = 1000;
	//Player Gravity
	float G = -1*GetMovementComponent()->GetGravityZ();
	float initialVelocity = FMath::Sqrt(2 * G * testHeight);
	float T = initialVelocity / G;
	
	LaunchCharacter(initialVelocity * GetActorUpVector(), 0, 0);
	Delay(T, "EnableSwing");
}
void ADemonCharacter::EnableSwing()
{
	OrbitPoint = GetActorForwardVector() * 300 + GetActorUpVector() * 300 + GetActorLocation();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	bEnableSwing = true;
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
	if (DodgeMontageArray.Num() > 0)
	{
		Log("Playing next dodge?");
		//PlayMontage(DodgeMontageArray[0]);
		
	}
	
	playerCanDodge = true;
}
void ADemonCharacter:: PlayerAttack()
{
	if (getHitReactionMontageIsActive())
	{
		Log("Player Hit");
		return;
	}
	if (!playerCanDodge)
	{
		Log("Player is dodging");
		return;
	}
	if (!playerCanAttck)
	{
		Log("Player cannot attack");
		return;
	}
	TArray<UAnimMontage*> AttackArray = AttackMontageArray;
	int AttackIndex = 0;
;	if (AttackArray.Num() == 0)
	{
		Log("Empty Attack array");
		return;
	}
	if (AttackArray.IsValidIndex(currentAttackIndex))
	{
		
		AttackIndex = currentAttackIndex;
	}
	else
	{
		currentAttackIndex = 0;
	}
	
	auto attackMontage = AttackArray[AttackIndex];

	if (playerEnemy)
	{
		auto rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), playerEnemy->GetActorLocation());
		auto newRot = GetActorRotation();
		newRot.Yaw = rot.Yaw;
		float moveToTime = attackMontage->GetDefaultBlendInTime();

		//MoveCharacterToRotationAndLocationIninterval(GetActorLocation(), newRot, moveToTime);
		SetActorRotation(newRot);
		float distanceFromEnemy = getDistanceFromCharacter(playerEnemy);
		if (distanceFromEnemy > attackRushMinimumDistance && 1)
		{
			PlayMontage(AttackRushMontage);
			BindMontage(AttackRushMontage, "AttackRushEnd");
			//MainCharacterAnimInstance->bindMontageRootMotionModifier(AttackRushMontage, 0.5);
			return;
		}
	}
	GetCharacterMovement()->StopMovementImmediately();
	TArray<FString> AttackTags;
	if (AttackMontageMap.Contains(attackMontage))
	{
		FString str = *AttackMontageMap.Find(attackMontage);
		str.ParseIntoArray(AttackTags, TEXT(","), true);
	}
	else
	{
		Log("Montage has no tags");
	}
	
	PlayMontage(attackMontage);
	if (AttackTags.Contains("MirrorLowerArm"))
	{
		Log("Mirroring lower arm");
		LowerArmState = ELowerArmState::LAS_Mirror;
	}
	else if (AttackTags.Contains("DisableLowerArm"))
	{
		LowerArmState = ELowerArmState::LAS_Disabled;
	}
	else
	{
		LowerArmState = ELowerArmState::LAS_Normal;
	}

	if (AttackTags.Contains("Mirror"))
	{
		Log("Miroring charaacter");
		MainCharacterAnimInstance->setEnableMirror(true);

	}
	
	BindMontage(attackMontage, "PlayerAttackEnd");

	playerCanAttck = false;
	currentAttackIndex++;
	
}
void ADemonCharacter::PlayerAttackEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	actorsHit.Empty();
	bEnableHitBox = false;

	if (bInterrupted)
	{
		
		
	}
	else
	{
		currentAttackIndex = 0;
	}
	LowerArmState = ELowerArmState::LAS_Normal;
	MainCharacterAnimInstance->setEnableMirror(false);
	playerCanAttck = true;
	
	
}
void ADemonCharacter::NextAttack()
{
	playerCanAttck = true;
}
void ADemonCharacter::onMoveStarted(const FInputActionValue& Value)
{
	Log("Move Started");
	if (GetCurrentMontage())
	{
		if (GetCurrentMontage() == TurnLeftMontage || GetCurrentMontage() == TurnRightMontage || 
			GetCurrentMontage() == TurnBackRightMontage || GetCurrentMontage() == TurnBackLeftMontage)
		{
			return;
		}
	}
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovingForwardValue = MovementVector.Y;
	MovingRightValue = MovementVector.X;

	bStartCharcterMovementRotation = true;
	currentCharacterRotationTime = 0.0;
	targetCurrentRotationTime = 1.0;

	characterInitialYawRotation = GetActorRotation().Yaw;
	characterFinalYawRotation = GetInputDirection().Rotation().Yaw;

	ETurnState turnState = TurnInPlace(GetInputDirection().Rotation());
	if (turnState != ETurnState::ETS_None)
	{
		switch (turnState) 
		{
				case ETurnState::ETS_Left:
					PlayMontage(TurnLeftMontage);
					break;
				case ETurnState::ETS_Right:
					PlayMontage(TurnRightMontage);
					break;
				case ETurnState::ETS_RightHalf:
					PlayMontage(TurnBackRightMontage);
					break;
				case ETurnState::ETS_LeftHalf:
					PlayMontage(TurnBackLeftMontage);
					break;
				default:
					break;
		}
		currentCharacterRotationTime = targetCurrentRotationTime;

		setCanCancelAnimMontage();
	}
	else
	{
		//Do something?
	}
}
void ADemonCharacter::onMoveEnd(const FInputActionValue& Value)
{
	Super::onMoveEnd(Value);
	//bStartCharcterMovementRotation = false;
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovingForwardValue = MovementVector.Y;
	MovingRightValue = MovementVector.X;
}
void ADemonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovingForwardValue = MovementVector.Y;
	MovingRightValue = MovementVector.X;

	if (MovementVector.Length() < 0.5)
	{
		return;
	}
	
	bStartCharcterMovementRotation = false;
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FRotator XYRotation = FRotator(0, Rotation.Yaw, Rotation.Roll);

		// get forward vector
		const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(YawRotation);

		// get right vector 
		const FVector RightDirection = UKismetMathLibrary::GetRightVector(XYRotation);


		// add movement 
		AddMovementInput(RightDirection, MovementVector.X);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}
	
	if (getCanCancelAnimMontage())
	{
		if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			auto currMont = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
			if (currMont)
			{
				GetMesh()->GetAnimInstance()->Montage_Stop(currMont->GetDefaultBlendOutTime(), currMont);
				setCanCancelAnimMontage(false);
			}
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
	if (getHitReactionMontageIsActive())
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
bool ADemonCharacter::getJumpButtonisPressed()
{
	return bJumpButtonIsPressed;
}
void ADemonCharacter::setJumpButtonisPressed(bool isPressed)
{
	bJumpButtonIsPressed = isPressed;
}
bool ADemonCharacter::getCharacterLanded()
{
	return bCharacterlanded;
}
void ADemonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	Log("Landed");
	if (!bIsClimbing)
	{
		PlayMontage(JumpLandMontage);
	}
	bCharacterlanded = true;
	bCycleRunnigJumpMirror = !bCycleRunnigJumpMirror;
	setCanCancelAnimMontage();
	ToggleBoost(true, false);
}
void ADemonCharacter::UpdateHitbox(float deltaTime, bool bEnableRightPunch, bool enableDebug)
{
	if (bEnableLimbHitBox)
	{
		AttackHitbox(limbAttackSocketName);
	}
}
void ADemonCharacter::StartHitbox(float deltaTime,bool bEnableRightPunch,bool enableDebug )
{
	if (!bEnableHitBox)
	{
		return;
	}

	//Should be one of the hand locations
	FName SocketName = "";
	TArray<FString> AttackTags;
	bool bEnableLeftKick = false;
	bool bEnableLeftPunch = false;
	bool bEnableRightKick = false;
	if (GetIsAttackAnimationPlaying())
	{
		FString str = *AttackMontageMap.Find(GetCurrentMontage());
		str.ParseIntoArray(AttackTags, TEXT(","), true);
		bEnableRightPunch = str.Contains("Right") && str.Contains("Punch");
		bEnableRightKick = str.Contains("Right") && str.Contains("Kick");
		bEnableLeftKick = str.Contains("Left") && str.Contains("Kick") ;
		bEnableLeftPunch = str.Contains("Left") && str.Contains("Punch");
	}
	
	bool containsAllSockets = GetMesh()->GetAllSocketNames().Contains(RightHandSocketName) && GetMesh()->GetAllSocketNames().Contains(LeftHandSocketName) &&
		GetMesh()->GetAllSocketNames().Contains(RightFootSocketName) && GetMesh()->GetAllSocketNames().Contains(LeftHandSocketName);

	if (!containsAllSockets)
	{
		Log(enableDebug ? "Not all attack sockets are named or not named correctyly":"");
		return;
	}

	if (bEnableRightPunch)
	{
		SocketName = RightHandSocketName;
	}
	if (bEnableLeftPunch)
	{
		SocketName = LeftHandSocketName;
	}
	if (bEnableLeftKick)
	{
		SocketName = LeftFootSocketName;
	}
	if (bEnableRightKick)
	{
		SocketName = RightFootSocketName;
	}
	AttackHitbox(SocketName);
}
void ADemonCharacter::UpdateHitbox(float deltaTime)
{
	if (!bEnableHitBox)
	{
		return;
	}
	//Should be one of the hand locations
	bool containsAllSockets = GetMesh()->GetAllSocketNames().Contains(AttackSocketName);

	if (!containsAllSockets)
	{
		Log("Sockets are not available or not named correctyly");
		return;
	}

	AttackHitbox(AttackSocketName);
}
void ADemonCharacter::AttackHitbox(FName SocketName)
{
	FVector AttackPoint = GetActorLocation();
	float zLoc = AttackPoint.Z;
	AttackPoint = GetMesh()->GetSocketLocation(SocketName);
	FVector AttackCenterPoint = AttackPoint;
	AttackCenterPoint.Z = GetActorLocation().Z;

	FVector AttackVector = AttackCenterPoint - GetActorLocation();

	float AttackVectorSize = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float AttackCenterDist = AttackVectorSize + GetCapsuleComponent()->GetScaledCapsuleRadius() + 50;

	AttackVector.Normalize();
	auto StartPoint = GetActorLocation();
	StartPoint.Z = AttackPoint.Z;
	auto midPoint = AttackCenterDist * AttackVector + GetActorLocation();

	auto start = midPoint;
	start.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	auto end = midPoint;
	end.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	auto radius = 20;

	FHitResult hitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPoint, StartPoint + AttackCenterDist * AttackVector, 
		radius, traceObjectTypes, false, actorsToIgnore, EDrawDebugTrace::None, hitResult, true);
	if (didHit)
	{
		if (AEnemy* enemy = Cast<AEnemy>(hitResult.GetActor()))
		{
			if (!actorsHit.Contains(enemy))
			{
				actorsHit.Add(enemy);
				enemy->HitReact(this);
				//SpawnParticle(HitImpact, hitResult.Location, FRotator::ZeroRotator);
				if (HitNiagraImpact)
				{
					float impactScale = 0.5;
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagraImpact, AttackPoint, 
						(StartPoint + AttackCenterDist * AttackVector).Rotation(), FVector(hitImpactSize), true, true, ENCPoolMethod::AutoRelease, true);
				}
				//DrawDebugSphere(GetWorld(), hitResult.Location, 25, 12, FColor::Blue, false, 2.0);
			}
		}
	}
}
bool ADemonCharacter::getHitReactionMontageIsActive() const
{
	return HitReactionMontage == GetCurrentMontage() || HitReactionMontageArray.Contains(GetCurrentMontage());
}
float ADemonCharacter::HitReact(AActor* sender)
{
	if (GetIsDodgeAnimationPlaying())
	{
		return 0.0;
	}
	if (!HitReactionMontage)
	{
		Log("HitRection montage no good");
	}
	auto vector = GetActorLocation() - sender->GetActorLocation();
	FName sectionName = "Default";
	vector.Normalize();
	vector *= -1;
	if (FVector::DotProduct(GetActorForwardVector(), vector) < FVector::DotProduct(-GetActorForwardVector(), vector))
	{
		for (FCompositeSection compSec : HitReactionMontage->CompositeSections)
		{
			if (compSec.SectionName.IsEqual("HitBack"))
			{
				sectionName = "HitBack";
			}
		}
	}
	
	auto hitReactMontage = HitReactionMontage;
	FOnMontageEnded BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ADemonCharacter::HitReactEnd);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, hitReactMontage);
	float T = PlayMontage(hitReactMontage, sectionName);
	if (ACharacter* character = Cast<ACharacter>(sender))
	{
		auto yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), character->GetActorLocation()).Yaw;

		auto newRot = GetActorRotation();
		newRot.Yaw = yaw;
		if (sectionName.IsEqual("HitBack"))
		{
			newRot.Yaw = character->GetActorRotation().Yaw;
		}
		//MoveCharacterToRotationAndLocationIninterval(GetActorLocation(), newRot, hitReactMontage->GetDefaultBlendInTime());
		SetActorRotation(newRot);
	}

	return T;
}
void ADemonCharacter::HitReactEnd(UAnimMontage* animMontage, bool bInterrupted)
{
}

void ADemonCharacter::AttackRushEnd(UAnimMontage* animMontage, bool bInterrupted)
{
}

void ADemonCharacter::EKeyActionPress()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;
	}
	else
	{
		//Needs work
		//StartWallClimb();
	}
}
void ADemonCharacter::StartWallClimb()
{
	FHitResult hitResult;
	bIsClimbing = SphereTrace(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 50, 25, ETraceTypeQuery::TraceTypeQuery1, 2, hitResult);
	if (!bIsClimbing)
	{
		return;
	}
	CLimbNormal = hitResult.Normal;
	GetCharacterMovement()->bCanWalkOffLedges = false;

}
void ADemonCharacter::UpdateWallclimb(float DeltaTime)
{
	FVector PlayerCurrentGravityDir = GetCharacterMovement()->GetGravityDirection();
	FVector TargetGravityDirection = bIsClimbing ? CLimbNormal * -1 : FVector(0, 0, -1);
	float targetSpeed = bIsClimbing ? 5.0 : 10.0;
	FVector newGravityDirection = UKismetMathLibrary::VInterpTo_Constant(PlayerCurrentGravityDir, TargetGravityDirection, DeltaTime, targetSpeed);
	GetCharacterMovement()->SetGravityDirection(newGravityDirection);

}
void ADemonCharacter::UpdateMovementRotation(float DeltatTime)
{
	if (bStartCharcterMovementRotation)
	{
		Log("Turning?");
		float ratio = currentCharacterRotationTime / targetCurrentRotationTime;
		if (GetCurrentMontage())
		{
			auto MyMontage = GetCurrentMontage();
			if (MainCharacterAnimInstance->GetCurveValue("CharacterRotationCurve", ratio))
			{
				float newYaw = UKismetMathLibrary::RLerp(FRotator(0, characterInitialYawRotation, 0), 
					FRotator(0, characterFinalYawRotation, 0), ratio, true).Yaw;
				
				SetActorRotation(FRotator(0, newYaw, 0));
				if (ratio >= 0.99)
				{
					SetActorRotation(FRotator(0, characterFinalYawRotation, 0));
					bStartCharcterMovementRotation = false;
				}
			}
			else
			{
				Log("Could not find curve value");
			}
			
		}
		else
		{
			if (currentCharacterRotationTime >= targetCurrentRotationTime)
			{
				float newYaw = UKismetMathLibrary::RLerp(FRotator(0, characterInitialYawRotation, 0),
					FRotator(0, characterFinalYawRotation, 0), ratio, true).Yaw;
				SetActorRotation(FRotator(0, characterFinalYawRotation, 0));
				bStartCharcterMovementRotation = false;
				return;
			}
			else
			{
				float newYaw = UKismetMathLibrary::RLerp(FRotator(0, characterInitialYawRotation, 0),
					FRotator(0, characterFinalYawRotation, 0), ratio, true).Yaw;
				SetActorRotation(FRotator(0, newYaw, 0));
				bStartCharcterMovementRotation = false;
			}
		}
		currentCharacterRotationTime += DeltatTime;
	}
}
bool ADemonCharacter::GetIsAttackAnimationPlaying()
{
	if (!GetCurrentMontage())
	{
		return false;
	}
	return AttackMontageMap.Contains(GetCurrentMontage());
}
bool ADemonCharacter::GetIsFreeflowAnimationPlaying()
{
	if (!GetCurrentMontage())
	{
		return false;
	}
	return FreeflowAttackMontageMap.Contains(GetCurrentMontage());
}
bool ADemonCharacter::GetIsDodgeAnimationPlaying()
{
	if (!GetCurrentMontage())
	{
		return false;
	}
	return DodgeMontageArray.Contains(GetCurrentMontage());
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