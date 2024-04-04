﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "VRGameMode.h"

#include "LabyrAInthVRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Network/LabyrinthDTO.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Core_Log);

AVRGameMode::AVRGameMode()
{
	// Set default classes for player controller
	PlayerControllerClass = AVRPlayerController::StaticClass();
	PlayerController = nullptr;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/VRCore/Blueprint/VR/VRCharacter"));
	DefaultPawnClass = PlayerPawnBPClass.Class;
	VRCharacter = nullptr;

	GameStateClass = AVRGameState::StaticClass();
	VRGameState = nullptr;

	NetworkController = nullptr;
	WidgetController = nullptr;
	SceneController = nullptr;

	LabyrinthDTO = nullptr;
}

void AVRGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Store references to the default player controller and pawn
	PlayerController = Cast<AVRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(PlayerController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of PlayerController"));
		throw "Invalid creation of PlayerController";
	}
	
	VRCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!IsValid(VRCharacter))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of Pawn"));
		throw "Invalid creation of Pawn";
	}

	// Spawn and set up network manager
	NetworkController = GetWorld()->SpawnActor<ANetworkController>();
	if (!IsValid(NetworkController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of NetworkController"));
		throw "Invalid creation of NetworkController";
	}
    
	// Spawn and set up game state
	// VRGameStateClass = GetWorld()->SpawnActor<AVRGameState>();
	VRGameState = GetWorld()->GetGameState<AVRGameState>();
	if (!IsValid(VRGameState))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of VRGameState"));
		throw "Invalid creation of VRGameState";
	}
	GameState = VRGameState;
    
	// Spawn and set up widget controller
	WidgetController = GetWorld()->SpawnActor<AWidgetController>();
	if (!IsValid(WidgetController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of WidgetController"));
		throw "Invalid creation of WidgetController";
	}
	
	// Spawn and set up scene controller
	SceneController = GetWorld()->SpawnActor<ASceneController>(SceneController_BP);
	if (!IsValid(SceneController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of SceneController"));
		throw "Invalid creation of SceneController";
	}

	// create a LabyrinthDTO
	LabyrinthDTO = NewObject<ULabyrinthDTO>();
	LabyrinthDTO->Level = 1;
	LabyrinthDTO->LabyrinthStructure.resize(11, std::vector<uint8>(11, 0));
	if (!IsValid(LabyrinthDTO))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of LabyrinthDTO"));
		throw "Invalid creation of LabyrinthDTO";
	}

	MainMenuLogicHandler();
}

void AVRGameMode::MainMenuLogicHandler()
{
	// Set up the game to be in Main Menu
	VRGameState->SetStateOfTheGame(EGameState::Egs_InMainMenu);
	WidgetController->OnNewGameButtonClicked.AddUObject(this, &AVRGameMode::OnNewGameButtonClicked);
	// WidgetController->ShowMainMenu();
}

void AVRGameMode::OnNewGameButtonClicked()
{
	// Set up the game to be in Waiting For Labyrinth state
	VRGameState->SetStateOfTheGame(EGameState::Egs_WaitingForLabyrinth);
	
	WidgetController->ShowLoadingScreen();
	NetworkController->OnLabyrinthReceived.AddUObject(this, &AVRGameMode::PrepareGame);
	NetworkController->OnNetworkError.AddUObject(this, &AVRGameMode::MockNetwork);
	NetworkController->GetLabyrinthFromBE(LabyrinthDTO);
}

void AVRGameMode::MockNetwork()
{
	LabyrinthDTO->Level = LabyrinthDTO->GetDefaultLevel();
	LabyrinthDTO->LabyrinthStructure = LabyrinthDTO->GetDefaultLabyrinthStructure();
	UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("NetworkError, using mocked Labyinth "));
	PrepareGame();
}


void AVRGameMode::PrepareGame()
{
	SceneController->OnSceneReady.AddUObject(this, &AVRGameMode::StartGame);
	const FString ErrorMessage = SceneController->SetupLevel(LabyrinthDTO);
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal Scene error: %s"), *ErrorMessage);
		throw ErrorMessage;
	}
}

void AVRGameMode::StartGame()
{
	// Set up the game to be in Playing state
	VRGameState->SetStateOfTheGame(EGameState::Egs_Playing);
	WidgetController->ShowGameUI();
	PlayerController->EnableInputs(true);
}

void AVRGameMode::OnPauseButtonClicked()
{
}

void AVRGameMode::OnEndGameButtonClicked()
{
}

void AVRGameMode::OnRestartGameButtonClicked()
{
}

void AVRGameMode::PauseGame()
{
}

void AVRGameMode::EndGame()
{
}

void AVRGameMode::RestartGame()
{
}

void AVRGameMode::ProceedToNextLevel()
{
}
