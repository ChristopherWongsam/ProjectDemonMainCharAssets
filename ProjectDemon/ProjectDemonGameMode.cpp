// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectDemonGameMode.h"
#include "ProjectDemonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectDemonGameMode::AProjectDemonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
