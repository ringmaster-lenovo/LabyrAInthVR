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

	bIsVRHMDConnected = IsVRHMDConnected();
	if (!bIsVRHMDConnected)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("No VR HMD connected: Switching to non-VR mode"));
		
		DefaultPawnClass = Character3DClass;
		Character3D = Cast<AMain3DCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!IsValid(Character3D))
		{
			UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of Pawn: change default pawn in GM_VRGameMode"));
			// throw "Invalid creation of Pawn";
		}
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
			// throw "Invalid creation of Pawn";
		}
	}

	BasePlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(BasePlayerController))
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Invalid creation of PlayerController"));
		throw "Invalid creation of PlayerController";
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
	WidgetController = GetWorld()->SpawnActor<AWidgetController>(BP_WidgetController);
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
	if (VRGameState->GetCurrentStateOfTheGame() == EGameState::Egs_InMainMenu)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Start Lobby, but game is already in Main Menu"));
		return;
	}
	// Set up the game to be in Main Menu
	VRGameState->SetStateOfTheGame(EGameState::Egs_InMainMenu);
	
	WidgetController->OnNewGameButtonClicked.AddUObject(this, &AVRGameMode::NewGameButtonClicked);
	WidgetController->ShowMainMenu();
	MusicController->StartAmbienceMusic(true);
}

void AVRGameMode::NewGameButtonClicked()
{
	if (VRGameState->GetCurrentStateOfTheGame() >= EGameState::Egs_WaitingForLabyrinth)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("New Game button clicked, but game is already waiting for labyrinth"));
		return;
	}
	// Set up the game to be in Waiting For Labyrinth state
	VRGameState->SetStateOfTheGame(EGameState::Egs_WaitingForLabyrinth);
	
	WidgetController->ShowLoadingScreen();
	MockNetwork();
	// NetworkController->OnLabyrinthReceived.AddUObject(this, &AVRGameMode::PrepareGame);
	// NetworkController->OnNetworkError.AddUObject(this, &AVRGameMode::MockNetwork);
	// NetworkController->GetLabyrinthFromBE(LabyrinthDTO);
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
	if (VRGameState->GetCurrentStateOfTheGame() >= EGameState::Egs_WaitingForSceneBuild)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Prepare Game, but game is already waiting for scene build"));
		return;
	}
	// Set up the game to be in Waiting For Labyrinth state
	VRGameState->SetStateOfTheGame(EGameState::Egs_WaitingForSceneBuild);
	
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
	if (VRGameState->GetCurrentStateOfTheGame() >= EGameState::Egs_Playing)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Start Game, but game is already playing"));
		return;
	}
	// Set up the game to be in Playing state
	VRGameState->SetStateOfTheGame(EGameState::Egs_Playing);
	
	WidgetController->ShowGameUI();
	MusicController->StartAmbienceMusic(false);
	// MusicController->StopCombatMusic();
	
	FVector PlayerStartPosition;
	FRotator PlayerStartRotation;
	SceneController->GetPlayerStartPositionAndRotation(PlayerStartPosition, PlayerStartRotation);
	const FString ErrorMessage = BasePlayerController->TeleportPlayer(PlayerStartPosition, PlayerStartRotation, true);
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal PLayer error: %s"), *ErrorMessage);
		throw ErrorMessage;
	}
	BasePlayerController->OnCollisionWithEndPortal.AddUObject(this, &AVRGameMode::EndGame);
}

void AVRGameMode::PauseButtonClicked()
{
}

void AVRGameMode::EndGameButtonClicked()
{
}

void AVRGameMode::RestartGameButtonClicked()
{
}

void AVRGameMode::PauseGame()
{
}

void AVRGameMode::EndGame()
{
	if (VRGameState->GetCurrentStateOfTheGame() >= EGameState::Egs_Ending)
	{
		UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("End Game, but game is already ending"));
		return;
	}
	// Set up the game to be in Ending state
	VRGameState->SetStateOfTheGame(EGameState::Egs_Ending);
	
	SceneController->OnSceneCleanedUp.AddUObject(this, &AVRGameMode::StartLobby);
	FString ErrorMessage = SceneController->CleanUpLevel();
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal Error: cannot clean scene at the end of a game"));
		throw(ErrorMessage);
	}
	
	AActor* StartActor = FindPlayerStart(BasePlayerController, "LobbyStart");
	FVector PlayerStartPosition = StartActor->GetActorLocation();
	FRotator PlayerStartRotation = StartActor->GetActorRotation();
	ErrorMessage = BasePlayerController->TeleportPlayer(PlayerStartPosition, PlayerStartRotation, false);
	if (ErrorMessage != "")
	{
		UE_LOG(LabyrAInthVR_Core_Log, Error, TEXT("Fatal Error: cannot teleport player back to lobby"));
		// throw(ErrorMessage);
	}
	if (bIsVRHMDConnected)
	{
		AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(BasePlayerController->GetCharacter());
		VRCharacter->IsInLobby = true;
		VRCharacter->SpawnPointer();
	}
}

void AVRGameMode::RestartGame()
{
}

void AVRGameMode::ProceedToNextLevel()
{
}


void AVRGameMode::CrashCloseGame()
{
	// Close the game
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
