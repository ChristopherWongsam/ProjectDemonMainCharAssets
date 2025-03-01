// Fill out your copyright notice in the Description page of Project Settings.


#include <Kismet/KismetSystemLibrary.h>
#include "BasicEnemy.h"

void ABasicEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHeadFindPlayer(DeltaTime);
	UpdateMoveToPlayer(DeltaTime);
}

void ABasicEnemy::UpdateHeadFindPlayer(float DeltaTime)
{
	if (bPlayerFound)
	{
		return;
	}
	if (!GetMesh()->DoesSocketExist(headForwardEndSocketName) || !GetMesh()->DoesSocketExist(headForwardStartSocket))
	{
		Log("Cannot look at player: Socket in skeletons does not exist");
	}
	FVector headOrigin = GetMesh()->GetSocketLocation(headForwardStartSocket);
	FVector headEndOrigin = GetMesh()->GetSocketLocation(headForwardEndSocketName);
	FVector EndPoint = headEndOrigin - headOrigin;
	EndPoint.Normalize();
	FHitResult hitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects(this, headOrigin, GetActorForwardVector() * 2000 + headOrigin, 25, 
																	traceObjectTypes, false, actorsToIgnore, EDrawDebugTrace::None, hitResult, true);

	if (didHit)
	{
		if (ADemonCharacter* enemy = Cast<ADemonCharacter>(hitResult.GetActor()))
		{
			Log("Main Character found");
			bPlayerFound = true;
			EnemyController->MoveToActor(hitResult.GetActor(), 20.0);
		}
	}
}
void ABasicEnemy::UpdateMoveToPlayer(float DeltaTime)
{
	if (!MyPlayerCharacter)
	{
		return;
	}
	if (!bPlayerFound)
	{
		return;
	}

	float playerToEnemyDistance = FVector::Dist(MyPlayerCharacter->GetActorLocation(), GetActorLocation());
	float yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MyPlayerCharacter->GetActorLocation()).Yaw;

	SetActorRotation(FRotator(0, yaw, 0));
	if (playerToEnemyDistance > EnemyAttackRange)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500;
		EnemyController->MoveToActor(MyPlayerCharacter, 1.0);
		bEnablePlayerRangeDecsion = true;
		EnemyMoveToLocation = GetActorLocation();
		CancelAllDelay();
	}
	else
	{
		EnemyController->StopMovement();
	}

	if (playerToEnemyDistance <= EnemyAttackRange)
	{
		if (!bEnablePlayerRangeDecsion)
		{
			return;
		}
		if (getSpeed() == 0.0 && !EnemyAnimInstance->Montage_IsActive(DodgeMontage) && !EnemyAnimInstance->Montage_IsActive(LightAttack))
		{
			int chance = UKismetMathLibrary::RandomInteger64InRange(0, 1);
			if (0)
			{
				if (DodgeMontage) 
				{
					FName dodgeSection = UKismetMathLibrary::RandomInteger64InRange(0, 1) == 0 ? "Default" : "DodgeRight";
					PlayMontage(DodgeMontage, dodgeSection);
					BindMontage(DodgeMontage, "OnDodgeEnd");
				}
			}
			else if(0)
			{
				bEnablePlayerRangeDecsion = false;
				float waitTime = UKismetMathLibrary::RandomFloatInRange(1.0, 2.0);
				Delay(waitTime, "EnablePlayerRangeDecision");
			}
			else
			{
				Attack();
			}
		}
	}
}
void ABasicEnemy::OnDodgeEnd(UAnimMontage* Montage, bool interrupted)
{
	LogScreen("Dodge end");
	int chance = UKismetMathLibrary::RandomInteger64InRange(0, 2);
	if (chance!=0)
	{
		Attack();
	}
	else
	{
		FName dodgeSection = UKismetMathLibrary::RandomInteger64InRange(0, 1) == 0 ? "Default" : "DodgeRight";
		PlayMontage(DodgeMontage, dodgeSection);
		BindMontage(DodgeMontage, "OnDodgeEnd");
	}
}

float ABasicEnemy::Attack()
{
	const float T = PlayMontage(LightAttack);
	setCanHitReact(false);
	return T;
}
void ABasicEnemy::EnablePlayerRangeDecision()
{
	LogScreen("Enable trigger EnablePlayerRangeDecision");
	bEnablePlayerRangeDecsion = true;
}
FVector ABasicEnemy::GetStrafeLocation()
{
	FVector ForwardLoc = GetActorForwardVector() * 50;
	int chance = UKismetMathLibrary::RandomInteger64InRange(0, 1);
	FVector RightLoc = GetActorRightVector() * 200;
	if (chance)
	{
		RightLoc *= -1;
	}

	FVector location = ForwardLoc + RightLoc + GetActorLocation();
	return location;
}
bool ABasicEnemy::GetIsAttackAnimationPlaying()
{
	return GetCurrentMontage() == LightAttack;
}
bool ABasicEnemy::GetIsDodgeAnimationPlaying()
{
	return GetCurrentMontage() == DodgeMontage;
}
float ABasicEnemy::HitReact(AActor* sender)
{
	if (!getCanHitReact())
	{
		LogScreen("Can't hit enemy: enemy is Attacking");
		return 0.0;
	}
	if (GetIsDodgeAnimationPlaying() && false)
	{
		LogScreen("Can't hit enemy: enemy is dodging");
		return 0.0;
	}
	int index = 0;
	if (HitReactMontageArray.Num() == 0)
	{
		LogScreen("Enemy hit react montage array empty");
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
	float T = PlayMontage(hitReactMontage, sectionName);
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
