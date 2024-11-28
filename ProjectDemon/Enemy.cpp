// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

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
		Log("Enemy Overlay material not set", false);
	}
}
