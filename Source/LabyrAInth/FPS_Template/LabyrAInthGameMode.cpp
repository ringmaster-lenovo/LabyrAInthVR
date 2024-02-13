// Copyright Epic Games, Inc. All Rights Reserved.

#include "LabyrAInthGameMode.h"
#include "LabyrAInthCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALabyrAInthGameMode::ALabyrAInthGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
