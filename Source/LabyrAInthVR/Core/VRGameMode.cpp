// Fill out your copyright notice in the Description page of Project Settings.


#include "VRGameMode.h"
#include "Kismet/GameplayStatics.h"

AVRGameMode::AVRGameMode()
{
	// Set default classes for player controller
	PlayerControllerClass = AVRPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/VRTemplate/Blueprints/VRPawn"));
	DefaultPawnClass = PlayerPawnBPClass.Class;

	GameStateClass = AVRGameState::StaticClass();
}

void AVRGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Store references to the default player controller and pawn
	DefaultPlayerController = Cast<AVRPlayerController>(GetWorld()->GetFirstPlayerController());
	
	DefaultPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Spawn and set up network manager
	NetworkController = GetWorld()->SpawnActor<ANetworkController>();
    
	// Spawn and set up game state
	// VRGameStateClass = GetWorld()->SpawnActor<AVRGameState>();
	GameState = GetWorld()->GetGameState<AVRGameState>();
    
	// Spawn and set up widget controller
	WidgetController = GetWorld()->SpawnActor<AWidgetController>();
	
	// Spawn and set up scene controller
	SceneController = GetWorld()->SpawnActor<ASceneController>();
}