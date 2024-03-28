#include "SceneController.h"

#include "LabyrinthParser.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

ASceneController::ASceneController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASceneController::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString ASceneController::SetupLevel(const ULabyrinthDTO* LabyrinthDTO) const
{
	if (!IsValid(Cast<UObject>(LabyrinthDTO))) return "Invalid LabyrinthDTO";

	ALabyrinthParser* LabyrinthParser = Cast<ALabyrinthParser>(
		UGameplayStatics::GetActorOfClass(this, ALabyrinthParser::StaticClass()));

	if (!IsValid(LabyrinthParser)) return "Invalid LabyrinthParser";

	if (LabyrinthParser->BuildLabyrinth(LabyrinthDTO->LabyrinthStructure))
	{
		OnSceneReady.Broadcast();
		return "";
	}
		
	return "Cannot Build Labyrinth";
}
