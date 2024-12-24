// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAIController.h"
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
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	StartHitbox(DeltaTime, true,true);
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
		bEnableLeftKick = str.Contains("Left") && str.Contains("Kick");
		bEnableLeftPunch = str.Contains("Left") && str.Contains("Punch");
	}

	bool containsAllSockets = GetMesh()->GetAllSocketNames().Contains(RightHandSocketName) && GetMesh()->GetAllSocketNames().Contains(LeftHandSocketName) &&
		GetMesh()->GetAllSocketNames().Contains(RightFootSocketName) && GetMesh()->GetAllSocketNames().Contains(LeftHandSocketName);

	if (!containsAllSockets)
	{
		Log("Not all attack sockets are named or not named correctyly");
		return;
	}

	if (bEnableRightPunch)
	{
		SocketName = RightHandSocketName;
		AttackHitbox(SocketName);
	}
	if (bEnableLeftPunch)
	{
		SocketName = LeftHandSocketName;
		AttackHitbox(SocketName);
	}
	if (bEnableLeftKick)
	{
		SocketName = LeftFootSocketName;
		AttackHitbox(SocketName);
	}
	if (bEnableRightKick)
	{
		SocketName = RightFootSocketName;
		AttackHitbox(SocketName);
	}
	
}

void AEnemy::AttackHitbox(FName SocketName)
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
	bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPoint, StartPoint + AttackCenterDist * AttackVector, radius, traceObjectTypes, false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true);
	if (didHit)
	{
		if (ADemonCharacter* enemy = Cast<ADemonCharacter>(hitResult.GetActor()))
		{
			if (!actorsHit.Contains(enemy))
			{
				actorsHit.Add(enemy);
				enemy->HitReact(this);
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
	auto vector = GetActorLocation() - sender->GetActorLocation();
	FName sectionName = "Default";
	vector.Normalize();
	vector *= -1;
	if (FVector::DotProduct(GetActorForwardVector(), vector) < FVector::DotProduct(-GetActorForwardVector(), vector))
	{
		sectionName = "HitBack";
	}
	if (HitReactMontageArray.IsValidIndex(hitReactionCounter))
	{
		index = hitReactionCounter;
	}
	else
	{
		hitReactionCounter = 0;
	}
	auto hitReactMontage = HitReactMontageArray[index];
	FOnMontageEnded BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &AEnemy::HitReactEnd);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, hitReactMontage);
	float T = PlayMontage(hitReactMontage,sectionName);
	bEnableMirrorAnimation = !bEnableMirrorAnimation;
	hitReactionCounter++;
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

	}
	else
	{
		hitReactionCounter = 0;
	}
}

void AEnemy::setEnableHitbox(bool enableHitbox)
{
	bEnableHitBox = enableHitbox;
}

void AEnemy::RestartHitbox()
{
	actorsHit.Empty();
	setEnableHitbox(false);
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
	return AttackMontageMap.Contains(GetCurrentMontage());
}