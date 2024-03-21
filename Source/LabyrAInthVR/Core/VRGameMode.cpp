// Fill out your copyright notice in the Description page of Project Settings.


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

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/VRTemplate/Blueprints/VRPawn"));
	DefaultPawnClass = PlayerPawnBPClass.Class;
	Pawn = nullptr;

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
	
	Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Spawn and set up network manager
	NetworkController = GetWorld()->SpawnActor<ANetworkController>();
    
	// Spawn and set up game state
	// VRGameStateClass = GetWorld()->SpawnActor<AVRGameState>();
	VRGameState = GetWorld()->GetGameState<AVRGameState>();
	GameState = VRGameState;
    
	// Spawn and set up widget controller
	WidgetController = GetWorld()->SpawnActor<AWidgetController>();
	
	// Spawn and set up scene controller
	SceneController = GetWorld()->SpawnActor<ASceneController>();

	// create a LabyrinthDTO
	LabyrinthDTO = NewObject<ULabyrinthDTO>();
	LabyrinthDTO->Level = 1;
	LabyrinthDTO->LabyrinthStructure.resize(10, std::vector<uint8>(10, 0));

	MainMenuLogicHandler();
}

void AVRGameMode::MainMenuLogicHandler()
{
	// Set up the game to be in Main Menu
	VRGameState->SetStateOfTheGame(EGameState::InMainMenu);
	WidgetController->OnNewGameButtonClicked.AddUObject(this, &AVRGameMode::OnNewGameButtonClicked);
	WidgetController->ShowMainMenu();
}

void AVRGameMode::OnNewGameButtonClicked()
{
	// Set up the game to be in Waiting For Labyrinth state
	VRGameState->SetStateOfTheGame(EGameState::WaitingForLabyrinth);
	
	WidgetController->ShowLoadingScreen();
	NetworkController->OnLabyrinthReceived.AddUObject(this, &AVRGameMode::PrepareGame);
	NetworkController->GetLabyrinthFromBE(LabyrinthDTO);
	/*if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Falat Network Error: %s"), *ErrorMessage);
		throw ErrorMessage;
	}*/
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
	VRGameState->SetStateOfTheGame(EGameState::Playing);
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
