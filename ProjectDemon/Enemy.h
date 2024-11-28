// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "ProjectDemonCharacter.h"

#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDEMON_API AEnemy : public AProjectDemonCharacter
{
	GENERATED_BODY()

public:

	UMaterialInterface* currMaterial;
	void enableOutline(bool enableOutline);
	bool enemyIsHighlighted();
	virtual void BeginPlay() override;


};
