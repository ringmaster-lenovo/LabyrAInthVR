#include "SceneController.h"

ASceneController::ASceneController()
{
	PrimaryActorTick.bCanEverTick = false;
	// LabyrinthParser = nullptr;
	// SpawnManager = nullptr;
}

void ASceneController::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString ASceneController::SetupLevel(ULabyrinthDTO* LabyrinthDTO)
{
	if (!IsValid(Cast<UObject>(LabyrinthDTO))) return "Invalid LabyrinthDTO";

	if (!IsValid(LabyrinthParser_BP) || !IsValid(SpawnManager_BP)) return "LabyrinthParser or SpawnManager not set in SceneController";

	// Instantiate the LabyrinthParser and build the labyrinth
	ALabyrinthParser* LabyrinthParser = GetWorld()->SpawnActor<ALabyrinthParser>(LabyrinthParser_BP);
	if (!IsValid(LabyrinthParser)) return "Invalid LabyrinthParserActor";
	
	// Instantiate the SpawnManager and spawn the actors in the labyrinth
	ASpawnManager* SpawnManager = GetWorld()->SpawnActor<ASpawnManager>(SpawnManager_BP);
	if (!IsValid(SpawnManager)) return "Invalid SpawnManagerActor";
	
	FString ErrorMessage = LabyrinthParser->BuildLabyrinth(LabyrinthDTO, SpawnManager);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}

	ErrorMessage = SpawnManager->SpawnActorsInLabyrinth(LabyrinthDTO);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	
	OnSceneReady.Broadcast();
	return "";
}
