// Copyright Epic Games, Inc. All Rights Reserved.

#include "PhysicsExperimentsGameMode.h"
#include "PhysicsExperimentsCharacter.h"
#include "UObject/ConstructorHelpers.h"

APhysicsExperimentsGameMode::APhysicsExperimentsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
