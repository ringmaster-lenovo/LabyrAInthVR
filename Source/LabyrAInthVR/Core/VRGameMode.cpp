// Fill out your copyright notice in the Description page of Project Settings.


#include "VRGameMode.h"

#include "LabyrAInthVRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "LabyrAInthVR/Music/MusicController.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "LabyrAInthVR/Player/Main3DCharacter.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Core_Log);

AVRGameMode::AVRGameMode()
{
	// Set default classes for player controller
	PlayerControllerClass = ABasePlayerController::StaticClass();
	BasePlayerController = nullptr;

	CharacterVRClass = AVRMainCharacter::StaticClass();
	Character3DClass = AMain3DCharacter::StaticClass();
	Character3D = nullptr;
	CharacterVR = nullptr;
	bIsVRHMDConnected = false;

	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/VRCore/Blueprint/VR/VRCharacter"));
	// DefaultPawnClass = PlayerPawnBPClass.Class;
	// VRCharacter = nullptr;

	GameStateClass = AVRGameState::StaticClass();
	VRGameState = nullptr;

	NetworkController = nullptr;
	WidgetController = nullptr;
	SceneController = nullptr;
	MusicController = nullptr;

	LabyrinthDTO = nullptr;
}

void AVRGameMode::BeginPlay()
{
	Super::BeginPlay();

	BasePlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(BasePlayerController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of PlayerController"));
		throw "Invalid creation of PlayerController";
	}

	bIsVRHMDConnected = IsVRHMDConnected();
	if (!bIsVRHMDConnected)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("No VR HMD connected: Switching to non-VR mode"));
		
		DefaultPawnClass = Character3DClass;
		Character3D = Cast<AMain3DCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!IsValid(Character3D))
		{
			UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of Pawn: change default pawn in GM_VRGameMode"));
			CrashCloseGame();
			// throw "Invalid creation of Pawn";
		}
		BasePlayerController->SetControlledCharacter(Character3D);
	}
	else
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("VR HMD connected: Starting VR mode"));

		// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/VRCore/Blueprint/VR/VRCharacter"));
		DefaultPawnClass = CharacterVRClass;
		CharacterVR = Cast<AVRMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!IsValid(CharacterVR))
		{
			UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of Pawn, change default pawn in GM_VRGameMode"));
			CrashCloseGame();
			// throw "Invalid creation of Pawn";
		}
		BasePlayerController->SetControlledCharacter(CharacterVR);
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
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
    
	// Spawn and set up widget controller
	WidgetController = GetWorld()->SpawnActor<AWidgetController>();
	if (!IsValid(WidgetController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of WidgetController"));
		throw "Invalid creation of WidgetController";
	}
	WidgetController->OnWidgetSError.AddUObject(this, &AVRGameMode::CrashCloseGame);
	
	// Spawn and set up scene controller
	SceneController = GetWorld()->SpawnActor<ASceneController>(BP_SceneController);
	if (!IsValid(SceneController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of SceneController"));
		throw "Invalid creation of SceneController";
	}

	MusicController = GetWorld()->SpawnActor<AMusicController>(BP_MusicController);
	if (!IsValid(MusicController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of MusicController"));
	}

	// create a LabyrinthDTO
	LabyrinthDTO = NewObject<ULabyrinthDTO>();
	LabyrinthDTO->Level = 3;
	LabyrinthDTO->Width = 31;
	LabyrinthDTO->Height = 31;
	
	// LabyrinthDTO->LabyrinthStructure.resize(11, std::vector<uint8>(11, 0));
	if (!IsValid(LabyrinthDTO))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of LabyrinthDTO"));
		throw "Invalid creation of LabyrinthDTO";
	}

	StartLobby();
}

void AVRGameMode::StartLobby()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_NotYetStarted &&
		VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Ending)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Start Lobby, but game is already started"));
		return;
	}
	// Set up the game to be in Main Menu
	VRGameState->SetStateOfTheGame(EGameState::Egs_InMainMenu);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	WidgetController->ShowMainMenu();
	WidgetController->OnNewGameButtonClicked.AddUObject(this, &AVRGameMode::NewGameButtonClicked);
	MusicController->StartAmbienceMusic(true);
}

void AVRGameMode::NewGameButtonClicked()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_InMainMenu)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("New Game button clicked, but game is not in main menu"));
		return;
	}
	// Set up the game to be in Waiting For Labyrinth state
	VRGameState->SetStateOfTheGame(EGameState::Egs_WaitingForLabyrinth);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	WidgetController->OnNewGameButtonClicked.RemoveAll(this);
	
	WidgetController->ShowLoadingScreen();
	MockNetwork();
	// NetworkController->OnLabyrinthReceived.AddUObject(this, &AVRGameMode::PrepareGame);
	// NetworkController->OnNetworkError.AddUObject(this, &AVRGameMode::MockNetwork);
	// NetworkController->GetLabyrinthFromBE(LabyrinthDTO);
}

void AVRGameMode::MockNetwork()
{
	NetworkController->OnNetworkError.RemoveAll(this);
	LabyrinthDTO->Level = LabyrinthDTO->GetDefaultLevel();
	LabyrinthDTO->LabyrinthStructure = LabyrinthDTO->GetDefaultLabyrinthStructure();
	UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("NetworkError, using mocked Labyinth "));
	PrepareGame();
}

void AVRGameMode::PrepareGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_WaitingForLabyrinth)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Prepare Game, but game was not waiting for labyrinth"));
		return;
	}
	// Set up the game to be in Waiting For Labyrinth state
	VRGameState->SetStateOfTheGame(EGameState::Egs_WaitingForSceneBuild);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	NetworkController->OnLabyrinthReceived.RemoveAll(this);
	
	SceneController->OnSceneReady.AddUObject(this, &AVRGameMode::StartGame);
	const FString ErrorMessage = SceneController->SetupLevel(LabyrinthDTO);
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal Scene error: %s"), *ErrorMessage);
		throw ErrorMessage;
	}
	// UFinishGameRequestDTO* FinishGameRequestDto = NewObject<UFinishGameRequestDTO>();
	// FinishGameRequestDto->username = TEXT("moli");
	// FinishGameRequestDto->score = 100;
	// NetworkController->FinishGame(FinishGameRequestDto);
}

void AVRGameMode::StartGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_WaitingForSceneBuild &&
		VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Restarting)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Start Game, but game was not waiting for scene build or restarting"));
		return;
	}
	// Set up the game to be in Playing state
	VRGameState->SetStateOfTheGame(EGameState::Egs_Playing);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	SceneController->OnSceneReady.RemoveAll(this);
	SceneController->OnActorsRespawned.RemoveAll(this);
	
	WidgetController->ShowGameUI();
	MusicController->StartAmbienceMusic(false);
	BasePlayerController->ResetPlayerStats();
	
	FVector PlayerStartPosition;
	FRotator PlayerStartRotation;
	SceneController->GetPlayerStartPositionAndRotation(PlayerStartPosition, PlayerStartRotation);
	const FString ErrorMessage = BasePlayerController->TeleportPlayer(PlayerStartPosition, PlayerStartRotation, true);
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal PLayer error: %s"), *ErrorMessage);
		throw ErrorMessage;
	}
	WidgetController->OnPauseEvent.AddUObject(this, &AVRGameMode::PauseGame);
	BasePlayerController->OnPLayerDeath.AddUObject(this, &AVRGameMode::EndGame, false);
	BasePlayerController->OnCollisionWithEndPortal.AddUObject(this, &AVRGameMode::EndGame, true);
}

void AVRGameMode::PauseGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Playing)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Pause Game, but game is not playing"));
		return;
	}
	VRGameState->SetStateOfTheGame(EGameState::Egs_Pausing);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	WidgetController->OnPauseEvent.RemoveAll(this);
	WidgetController->OnResumeGameEvent.AddUObject(this, &AVRGameMode::ResumeGame);
	WidgetController->OnRestartLevelEvent.AddUObject(this, &AVRGameMode::RestartGame);
	WidgetController->OnReturnToMainMenuEvent.AddUObject(this, &AVRGameMode::EndGame, false);
}

void AVRGameMode::ResumeGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Pausing)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Resume Game, but game is not in pause state"));
		return;
	}
	VRGameState->SetStateOfTheGame(EGameState::Egs_Playing);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	WidgetController->OnResumeGameEvent.RemoveAll(this);
	WidgetController->OnRestartLevelEvent.RemoveAll(this);
	WidgetController->OnReturnToMainMenuEvent.RemoveAll(this);
	WidgetController->OnPauseEvent.AddUObject(this, &AVRGameMode::PauseGame);
}

void AVRGameMode::EndGame(bool bIsWin)
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Playing &&
		VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Pausing)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("End Game, but game is not playing or in pause state"));
		return;
	}
	// Set up the game to be in Ending state
	VRGameState->SetStateOfTheGame(EGameState::Egs_Ending);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());

	// unbind all events
	BasePlayerController->OnPLayerDeath.RemoveAll(this);
	BasePlayerController->OnCollisionWithEndPortal.RemoveAll(this);
	WidgetController->OnPauseEvent.RemoveAll(this);
	WidgetController->OnResumeGameEvent.RemoveAll(this);
	WidgetController->OnRestartLevelEvent.RemoveAll(this);
	WidgetController->OnReturnToMainMenuEvent.RemoveAll(this);

	// teleport player back to lobby
	AActor* StartActor = FindPlayerStart(BasePlayerController, "LobbyStart");
	FVector PlayerStartPosition = StartActor->GetActorLocation();
	FRotator PlayerStartRotation = StartActor->GetActorRotation();
	FString ErrorMessage = BasePlayerController->TeleportPlayer(PlayerStartPosition, PlayerStartRotation, false);
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal Error: cannot teleport player back to lobby"));
		// throw(ErrorMessage);
		return;
	}

	if (bIsWin)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Player has won the game"));
		MusicController->StartFinalResultMusic(true);
		int32 TimeOnLevel = BasePlayerController->GetPlayerTimeOnCurrentLevel();
		FString PlayerName = BasePlayerController->GetPlayerName();
		UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("%s finished the game in %d seconds"), *PlayerName, TimeOnLevel);
		WidgetController->ShowWinScreen();
		// NetworkController->FinishGame(fill the DTOs);Y0
	}
	else
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Player has lost the game"));
		MusicController->StartFinalResultMusic(false);
		WidgetController->ShowLoseScreen();
		WidgetController->OnRestartLevelEvent.AddUObject(this, &AVRGameMode::RestartGame);
	}
	WidgetController->OnReturnToMainMenuEvent.AddUObject(this, &AVRGameMode::RePrepareGame);

}

void AVRGameMode::RestartGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Ending &&
		VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Pausing)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Restart Game, but game is not ending or pausing"));
		return;
	}
	VRGameState->SetStateOfTheGame(EGameState::Egs_Restarting);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	WidgetController->OnRestartLevelEvent.RemoveAll(this);
	WidgetController->OnReturnToMainMenuEvent.RemoveAll(this);

	SceneController->OnActorsRespawned.AddUObject(this, &AVRGameMode::StartGame);
	SceneController->RespawnMovableActors(LabyrinthDTO);
}

void AVRGameMode::RePrepareGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() != EGameState::Egs_Ending)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Restart Game, but game was not ending"));
		return;
	}
	VRGameState->SetStateOfTheGame(EGameState::Egs_Restarting);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());

	WidgetController->OnReturnToMainMenuEvent.RemoveAll(this);
	WidgetController->OnRestartLevelEvent.RemoveAll(this);
	
	SceneController->OnSceneCleanedUp.AddUObject(this, &AVRGameMode::StartLobby);
	FString ErrorMessage = SceneController->CleanUpLevel();
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal Error: cannot clean scene at the end of a game"));
		throw(ErrorMessage);
	}
}

void AVRGameMode::CrashCloseGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() == EGameState::Egs_ClosingGame)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Crash Close Game, but game is already closing"));
		return;
	}
	VRGameState->SetStateOfTheGame(EGameState::Egs_ClosingGame);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Active Game State: %s"), *VRGameState->GetCurrentStateOfTheGameAsString());
	
	// Close the game
	MusicController->StopMusic();
	FGenericPlatformMisc::RequestExit(false);
}

bool AVRGameMode::IsVRHMDConnected()
{
	// get the HMD information
	const FName HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
	FXRHMDData HMDData;
	UHeadMountedDisplayFunctionLibrary::GetHMDData(this, HMDData);
	const FString HMDVersion = UHeadMountedDisplayFunctionLibrary::GetVersionString();
	const bool HMDIsConnected = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
	const bool HMDIsEnabled = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();

	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("HMD Name: %s"), *HMDName.ToString());
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("HMD Device Name: %s"), *HMDData.DeviceName.ToString());
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("HMD ApplicationInstanceID: %s"), *HMDData.ApplicationInstanceID.ToString());
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("HMDVersion: %s"), *HMDVersion);
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("HMDIsConnected: %s"), HMDIsConnected ? TEXT("true") : TEXT("false") );
	UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("HMDIsEnabled: %s"), HMDIsEnabled ? TEXT("true") : TEXT("false") );
	
	return (HMDIsConnected && HMDIsEnabled);
}
