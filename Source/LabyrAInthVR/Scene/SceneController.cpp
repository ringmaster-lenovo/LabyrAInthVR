#include "SceneController.h"

#include "Config.h"
#include "LabyrinthParser.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

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

FString ASceneController::SetupLevel(const ULabyrinthDTO* LabyrinthDTO)
{
	if (!IsValid(Cast<UObject>(LabyrinthDTO))) return "Invalid LabyrinthDTO";

	ALabyrinthParser* LabyrinthParser = Cast<ALabyrinthParser>(
		UGameplayStatics::GetActorOfClass(this, ALabyrinthParser::StaticClass()));

	if (!IsValid(LabyrinthParser)) return "Invalid LabyrinthParser";
	
	if (LabyrinthParser->BuildLabyrinth(LabyrinthDTO->LabyrinthStructure))
	{
		// Adjust volume
		UpdateNavMeshBoundsVolume(LabyrinthDTO);
		OnSceneReady.Broadcast();
		return "";
	}
	
	return "Cannot Build Labyrinth";
}

void ASceneController::UpdateNavMeshBoundsPosition()
{
	NavMeshBoundsVolume->AddActorWorldOffset(FVector{1.f});
	NavMeshBoundsVolume->AddActorWorldOffset(FVector{-1.f});

	UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(this);

	if(!IsValid(NavigationSystemV1)) return;

	NavigationSystemV1->OnNavigationBoundsUpdated(NavMeshBoundsVolume);
}

void ASceneController::UpdateNavMeshBoundsVolume(const ULabyrinthDTO* LabyrinthDto)
{
	NavMeshBoundsVolume = Cast<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(this, ANavMeshBoundsVolume::StaticClass()));
	
	if(!IsValid(NavMeshBoundsVolume)) return;
		
	FVector NewScale {std::size(LabyrinthDto->LabyrinthStructure[0])*WallSettings::WallOffset,
	std::size(LabyrinthDto->LabyrinthStructure)*WallSettings::WallOffset,
	100.f};
	NavMeshBoundsVolume->SetActorScale3D(NewScale);
	NavMeshBoundsVolume->SetActorLocation(FVector{0.f, 0.f, 0.f});
	
	GetWorldTimerManager().SetTimer(NavMeshBoundsTimerHandle, this, &ASceneController::UpdateNavMeshBoundsPosition, 0.25f, false);
}
