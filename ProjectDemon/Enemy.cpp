// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AIController.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Runtime/Engine/Private/InterpolateComponentToAction.h>
#include "DemonCharacter.h"


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->GetOverlayMaterial();
	if (GetMesh()->GetOverlayMaterial())
	{
		currMaterial = GetMesh()->GetOverlayMaterial();
		GetMesh()->SetOverlayMaterial(nullptr);
	}
	else
	{
		PrintLog("Enemy Overlay material not set");
	}
	EnemyController = Cast<AEnemyAIController>(GetController());
	if (EnemyController)
	{
		PrintLog("Enemy controller set.");
	}
	else
	{
		PrintLog("Enemy controller not set!!!");
		return;
	}
	MyPlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (MyPlayerCharacter)
	{
		PrintLog("Enemy found main character!!!");
	}
	else
	{
		return;
	}
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	StartHitbox(DeltaTime, true,true);
	updateCharacterRotationToTarget(DeltaTime);
}

void AEnemy::LogScreen(FString log, FLinearColor color)
{
	Super::LogScreen(log, FLinearColor::Red);
}
void AEnemy::Log(FString log, bool printToScreen)
{
	Super::Log(log, printToScreen);
}
void AEnemy::enableOutline(bool enableOutline)
{
	GetMesh()->GetOverlayMaterial();
	if (enableOutline)
	{
		if (currMaterial)
		{
			GetMesh()->SetOverlayMaterial(currMaterial);
		}
		else
		{
			Log("Enemy has no current material", false);
		}
	}
	else
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}
}

bool AEnemy::enemyIsHighlighted()
{
	return GetMesh()->GetOverlayMaterial() != nullptr;
}

float AEnemy::Attack()
{
	LogScreen("Attacking Player", FLinearColor::Red);
	return PlayMontage(LightAttack);
}

void AEnemy::StartHitbox(float deltaTime, bool bEnableRightPunch, bool enableDebug)
{
	if (!GetIsAttackAnimationPlaying())
	{
		if (actorsHit.Num() > 0)
		{
			actorsHit.Empty();
		}
		return;
	}

	try
	{
		FName SocketName;
		TArray<FString> AttackTags;
		
		TArray<FAnimNotifyEvent> fAnimNotifyEvents = EnemyAnimInstance->ActiveAnimNotifyState;

		if (fAnimNotifyEvents.Num() == 0)
		{
			actorsHit.Empty();
		}
		
		bool isFound = false;

		for (FAnimNotifyEvent fAnimNotifyEvent : fAnimNotifyEvents)
		{
			//Log(fAnimNotifyEvent.NotifyName.ToString());
			FString notifyName = fAnimNotifyEvent.NotifyName.ToString();

			if (notifyName.Equals("LeftPunchNotifyState"))
			{
				SocketName = LeftHandSocketName;
			}
			if (notifyName.Equals("RightPunchNotifyState"))
			{
				SocketName = RightHandSocketName;
			}
			if (notifyName.Equals("LeftKickNotifyState"))
			{
				SocketName = LeftFootSocketName;
			}
			if (notifyName.Equals("RightKickNotifyState"))
			{
				SocketName = RightFootSocketName;
			}
			if (!SocketName.IsNone())
			{
				isFound = true;
				AttackHitbox(SocketName, true);
			}
			if (notifyName.Equals("SwordAttackNotifyState"))
			{
				SwordHitbox(this->RightWeaponSocketInitialPoint, this->RightWeaponSocketFinalPoint);
				isFound = true;
			}
		}

		if (!isFound)
		{
			actorsHit.Empty();
		}

	}
	catch (const std::exception& e)
	{
		Log(e.what());
	}
}

void AEnemy::updateCharacterRotationToTarget(float deltaTime, FVector targetLocation, bool enableDebug)
{
	if (!MyPlayerCharacter)
	{
		return;
	}
	if(!getEnableCharacterTargetRoataion())
	{ 
		return;
	}
	float newYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MyPlayerCharacter->GetActorLocation()).Yaw;
	if (!targetLocation.Equals(FVector::ZeroVector))
	{
		newYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetLocation).Yaw;
	}
	FRotator newRot(0, newYaw, 0);
	SetActorRotation(newRot);
	
}

void AEnemy::AttackHitbox(FName SocketName, bool bEnableDebug)
{
	FVector AttackPoint = GetActorLocation();
	float zLoc = AttackPoint.Z;
	AttackPoint = GetMesh()->GetSocketLocation(SocketName);
	FVector AttackCenterPoint = AttackPoint;
	AttackCenterPoint.Z = GetActorLocation().Z;

	FVector AttackVector = AttackCenterPoint - GetActorLocation();

	float AttackVectorSize = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float AttackCenterDist = AttackVectorSize + GetCapsuleComponent()->GetScaledCapsuleRadius() + 100;

	AttackVector.Normalize();
	auto StartPoint = GetActorLocation();
	StartPoint.Z = AttackPoint.Z;
	auto midPoint = AttackCenterDist * AttackVector + GetActorLocation();

	auto start = midPoint;
	start.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	auto end = midPoint;
	end.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	auto radius = 50;

	FHitResult hitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPoint, StartPoint + AttackCenterDist * AttackVector, radius, traceObjectTypes, false, actorsToIgnore, EDrawDebugTrace::None, hitResult, true);
	if (didHit)
	{
		if (ADemonCharacter* enemy = Cast<ADemonCharacter>(hitResult.GetActor()))
		{
			if (!actorsHit.Contains(enemy))
			{
				actorsHit.Add(enemy);
				enemy->HitReact(this);
				Log("Main Character hit");
				//DrawDebugSphere(GetWorld(), hitResult.Location, 25, 12, FColor::Blue, false, 2.0);
			}
		}
	}
}
void AEnemy::SwordHitbox(FName SocketInitName, FName SocketFinalName)
{
	FVector AttackInitPoint = GetMesh()->GetSocketLocation(SocketInitName);
	FVector AttackFinalPoint = GetMesh()->GetSocketLocation(SocketFinalName);

	auto radius = 50;

	FHitResult hitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, AttackInitPoint, AttackFinalPoint, radius, traceObjectTypes, false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true);
	if (didHit)
	{
		if (ADemonCharacter* enemy = Cast<ADemonCharacter>(hitResult.GetActor()))
		{
			if (!actorsHit.Contains(enemy))
			{
				actorsHit.Add(enemy);
				enemy->HitReact(this);
				Log("Main Character hit");
				DrawDebugSphere(GetWorld(), hitResult.Location, 25, 12, FColor::Blue, false, 2.0);
			}
		}
	}
}
float AEnemy::HitReact(AActor* sender)
{
	int index = 0;
	if (HitReactMontageArray.Num() == 0)
	{
		Log("Enemy hit react montage array empty");
		return 0.0;
	}
	if (GetCurrentMontage())
	{
		StopAnimMontage(GetCurrentMontage());
	}
	auto vector = GetActorLocation() - sender->GetActorLocation();
	FName sectionName = "Default";
	vector.Normalize();
	vector *= -1;
	if (FVector::DotProduct(GetActorForwardVector(), vector) < FVector::DotProduct(-GetActorForwardVector(), vector))
	{
		sectionName = "HitBack";
	}
	auto hitReactMontage = HitReactMontageArray[index];
	float T = PlayMontage(hitReactMontage,sectionName);
	FOnMontageEnded BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &AEnemy::HitReactEnd);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, hitReactMontage);
	if (ACharacter* character = Cast<ACharacter>(sender))
	{
		auto yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), character->GetActorLocation()).Yaw;

		auto newRot = GetActorRotation();
		newRot.Yaw = yaw;
		if (sectionName.IsEqual("HitBack"))
		{
			newRot.Yaw = character->GetActorRotation().Yaw;
		}
	}
	
	return T;
}
void AEnemy::HitReactEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	if (bInterrupted)
	{
		bEnableMirrorAnimation = !bEnableMirrorAnimation;

	}
	else
	{
		Log("Enemy hit react end");
		bEnableMirrorAnimation = false;

	}
}



void AEnemy::ChasePlayer()
{
	Log("Enemy Chasing Player");
	EnemyController->MoveToActor(MyPlayerCharacter, AcceptableAttackRange);
	Delay(0.2, "ChasePlayer");
}
bool AEnemy::GetIsAttackAnimationPlaying()
{
	if (!GetCurrentMontage())
	{
		return false;
	}
	return GetCurrentMontage() == LightAttack;
}