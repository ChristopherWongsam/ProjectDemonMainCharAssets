// Fill out your copyright notice in the Description page of Project Settings.


#include <Kismet/KismetSystemLibrary.h>
#include "BasicEnemy.h"

void ABasicEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ABasicEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHeadFindPlayer(DeltaTime);
	UpdateMoveToPlayer(DeltaTime);
}

void ABasicEnemy::UpdateHeadFindPlayer(float DeltaTime)
{
	if (bPlayerFound || !bEnemyCanAttack)
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

	TArray<AActor*> actors = GetActorsFromSphere(ADemonCharacter::StaticClass(), 2500);
	if (actors.Num() > 0)
	{
		if (ADemonCharacter* enemy = Cast<ADemonCharacter>(actors[0]))
		{
			Log("Main Character found");
			bPlayerFound = true;
			EnemyController->MoveToActor(hitResult.GetActor(), 20.0);
			setEnableCharacterToTargetRotation(true);
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
	
	if (playerToEnemyDistance > EnemyRadiusRange)
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

	if (playerToEnemyDistance <= EnemyRadiusRange)
	{
		

		if (!bEnablePlayerRangeDecsion)
		{
			return;
		}

		try
		{
			if (HitReactMontageArray.Contains(GetCurrentMontage()))
			{
				return;
			}
			if (playerToEnemyDistance <= EnemyAttackRange)
			{
				if (!EnemyAnimInstance->Montage_IsActive(LightAttack))
				{

					FRotator newRot = GetActorRotation();
					newRot.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MyPlayerCharacter->GetActorLocation()).Yaw;
					SetActorRotation(newRot);
					Attack();
					EnemyController->StopMovement();
					return;
				}
				EnemyController->StopMovement();
				return;
			}

			if (!EnemyAnimInstance->Montage_IsActive(LightAttack) &&
				!DodgMontageChainMap.Contains(EnemyAnimInstance->GetCurrentActiveMontage()))
			{
				UAnimMontage* dodgeMont = nullptr;
				//Log("Decision time");
				bool chance = UKismetMathLibrary::RandomBool();
				if (chance)
				{
					dodgeMont = DodgeFowLeftMontage;
				}
				else
				{
					dodgeMont = DodgeFowRightMontage;
				}

				if (dodgeMont && DodgMontageChainMap.Contains(dodgeMont))
				{

				}

				if (dodgeMont && DodgMontageChainMap.Contains(dodgeMont))
				{
					setEnableCharacterToTargetRotation(false);
					PlayMontage(dodgeMont);
					EnemyAnimInstance->bindMontageRootMotionModifier(dodgeMont, DodgeMovementScale);
					BindMontage(dodgeMont, "OnDodgeEnd");
				}
				else
				{
					EnemyController->MoveToActor(MyPlayerCharacter, 1.0);
				}
			}
		}
		catch (const std::exception& e)
		{
			Log(e.what());
		}
		
	}
}
void ABasicEnemy::OnDodgeEnd(UAnimMontage* Montage, bool interrupted)
{
	LogScreen("Dodge end");
	int chance = UKismetMathLibrary::RandomInteger64InRange(0, 2);
	setEnableCharacterToTargetRotation(false);

	float playerToEnemyDistance = 0.0;

	if (MyPlayerCharacter)
	{
		playerToEnemyDistance = FVector::Dist(MyPlayerCharacter->GetActorLocation(), GetActorLocation());
	}

	if (playerToEnemyDistance <= EnemyAttackRange)
	{
		return;
	}

	UAnimMontage* nextMontage = nullptr;

	if (DodgMontageChainMap.Contains(Montage))
	{
		FString montName;
		Montage->GetName(montName);
		nextMontage = *DodgMontageChainMap.Find(Montage);
		nextMontage->GetName(montName);
	}

	if (nextMontage && MyPlayerCharacter)
	{
		FRotator newRot = GetActorRotation();
		newRot.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MyPlayerCharacter->GetActorLocation()).Yaw;
		SetActorRotation(newRot);

		PlayMontage(nextMontage);
		EnemyAnimInstance->bindMontageRootMotionModifier(nextMontage, DodgeMovementScale);
		BindMontage(nextMontage, "OnDodgeEnd");
	}
}

float ABasicEnemy::Attack()
{
	if (LightAttack)
	{
		const float T = PlayMontage(LightAttack);
		setCanHitReact(false);
		return T;
	}
	return 0.0;
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

void ABasicEnemy::OnAttackEnd(UAnimMontage* Montage, bool interrupted)
{
	setEnableCharacterToTargetRotation(true);
}
