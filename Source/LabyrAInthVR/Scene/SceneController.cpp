#include "SceneController.h"

#include "LabyrinthParser.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Network/LabyrinthDTO.h"

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

bool ASceneController::SetupLevel(UObject* LabyrinthDTO) const
{
	if (!IsValid(LabyrinthDTO)) return false;

	ALabyrinthParser* LabyrinthParser = Cast<ALabyrinthParser>(
		UGameplayStatics::GetActorOfClass(this, ALabyrinthParser::StaticClass()));
	ULabyrinthDTO* Labyrinth = Cast<ULabyrinthDTO>(LabyrinthDTO);

	if (!IsValid(Labyrinth) || !IsValid(LabyrinthParser)) return false;

	if (LabyrinthParser->BuildLabyrinth(Labyrinth->LabyrinthStructure))
	{
		OnSceneReady.Broadcast();
		return true;
	}
	return false;
}
