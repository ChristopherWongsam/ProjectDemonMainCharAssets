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
	int index = UKismetMathLibrary::RandomInteger(AttackMontageArray.Num());
	return PlayMontage(LightAttack);
}


void AEnemy::ChasePlayer()
{
	Log("Enemy Chasing Player");
	EnemyController->MoveToActor(MyPlayerCharacter, AcceptableAttackRange);
	Delay(0.2, "ChasePlayer");
}
