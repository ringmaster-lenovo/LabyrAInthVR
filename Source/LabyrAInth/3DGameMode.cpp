// Copyright Epic Games, Inc. All Rights Reserved.

#include "3DGameMode.h"
#include "Character/3DPlayerController.h"
#include "UObject/ConstructorHelpers.h"

A3DGameMode::A3DGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = A3DPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/Character/BP_3DCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/Character/BP_3DPlayerController"));
	if(PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}