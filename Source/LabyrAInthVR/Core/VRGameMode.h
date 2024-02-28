// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LabyrAInthVR/Core/VRGameState.h"
#include "LabyrAInthVR/Network/NetworkController.h"
#include "LabyrAInthVR/Scene/SceneController.h"
#include "LabyrAInthVR/Widgets/WidgetController.h"
#include "LabyrAInthVR/Player/VRPlayerController.h"
#include "VRGameMode.generated.h"

/**
 * This is the heart of the game. It will control the game state and the game flow.
 * It controls the game mode to be used in a level. It also controls the game state.
 * It needs to communicate with all the other controllers of the game hence the NetworkController, the SceneController, the PlayerController, the WidgetController
 *  It is a singleton class, so it can be accessed from anywhere in the game, but only one instance of it can exist at a time, and it is not exposed to blueprints or to the editor.
*/
UCLASS()
class LABYRAINTHVR_API AVRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVRGameMode();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AVRPlayerController* DefaultPlayerController;

	UPROPERTY()
	APawn* DefaultPawn;

	UPROPERTY()
	ANetworkController* NetworkController;

	UPROPERTY()
	AVRGameState* VRGameStateClass;

	UPROPERTY()
	AWidgetController* WidgetController;

	UPROPERTY()
	ASceneController* SceneController;
};
