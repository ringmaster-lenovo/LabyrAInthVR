// Copyright Epic Games, Inc. All Rights Reserved.

#include "progettinoUniGameMode.h"
#include "progettinoUniPlayerController.h"
#include "progettinoUniCharacter.h"
#include "UObject/ConstructorHelpers.h"

AprogettinoUniGameMode::AprogettinoUniGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AprogettinoUniPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/Character/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/Character/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}