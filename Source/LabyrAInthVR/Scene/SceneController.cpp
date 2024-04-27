#include "SceneController.h"

#include "Config.h"
#include "EngineUtils.h"
#include "LabyrinthParser.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Pickups/BasePickup.h"
#include "LabyrAInthVR/Interagibles/PowerUp.h"
#include "LabyrAInthVR/Interagibles/Trap.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"
#include "NavMesh/NavMeshBoundsVolume.h"

ASceneController::ASceneController()
{
	PrimaryActorTick.bCanEverTick = false;
	NavMeshBoundsVolume = nullptr;
	LabyrinthParser = nullptr;
	SpawnManager = nullptr;
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
	LabyrinthParser = GetWorld()->SpawnActor<ALabyrinthParser>(LabyrinthParser_BP);
	if (!IsValid(LabyrinthParser)) return "Invalid LabyrinthParserActor";
	
	// Instantiate the SpawnManager and spawn the actors in the labyrinth
	SpawnManager = GetWorld()->SpawnActor<ASpawnManager>(SpawnManager_BP);
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
	
	UpdateNavMeshBoundsVolume(LabyrinthDTO);
	
	OnSceneReady.Broadcast();
	return "";
}

FString ASceneController::CleanUpOnlyLevel() const
{
	// Get a reference to the game world
	const UWorld* World = GetWorld();
	if (!World)
	{
		return "No valid world found";
	}
	
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor == nullptr) continue;

		// Check if the actor's class matches any of the classes to destroy
		if (Actor->Implements<USpawnableActor>())
		{
			Actor->Destroy();
		}
	}
	OnSceneCleanedUp.Broadcast();
	return "";
}

FString ASceneController::CleanUpLevelAndDoStatistics(int &NumOfEnemiesKilled, int &NumOfTrapsExploded, int &NumOfPowerUpsCollected, int &NumOfWeaponsFound) const
{
	// Get a reference to the game world
	const UWorld* World = GetWorld();
	if (!World)
	{
		return "No valid world found";
	}
	
	int NumOfEnemiesAlive = 0;
	int NumOfTrapsActive = 0;
	int NumOfPowerUpsNotCollected = 0;
	NumOfWeaponsFound = 0;
	int NumOfEnemiesSpawned = 0;
	int NumOfTrapsSpawned = 0;
	int NumOfPowerUpsSpawned = 0;
	int NumOfWeaponsSpawned = 0;
	SpawnManager->GetNumOfActorSpawned(NumOfEnemiesSpawned, NumOfTrapsSpawned, NumOfPowerUpsSpawned, NumOfWeaponsSpawned);
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor == nullptr) continue;

		// Check if the actor's class matches any of the classes to destroy
		if (Actor->Implements<USpawnableActor>())
		{
			if (Actor->IsA<ABaseEnemy>()) NumOfEnemiesAlive++;
			if (Actor->IsA<ATrap>()) NumOfTrapsActive++;
			if (Actor->IsA<APowerUp>()) NumOfPowerUpsNotCollected++;
			if (Actor->IsA<ABasePickup>())
			{
				ABasePickup* Pickup = Cast<ABasePickup>(Actor);
				if (Pickup != nullptr && Pickup->HasBeenFound()) NumOfWeaponsFound++;
			}
			Actor->Destroy();
		}
	}
	NumOfEnemiesKilled = NumOfEnemiesSpawned - NumOfEnemiesAlive;
	NumOfTrapsExploded = NumOfTrapsSpawned - NumOfTrapsActive;
	NumOfPowerUpsCollected = NumOfPowerUpsSpawned - NumOfPowerUpsNotCollected;
	return "";
}

FString ASceneController::RespawnMovableActors(ULabyrinthDTO* LabyrinthDto) const
{
	// Get a reference to the game world
	const UWorld* World = GetWorld();
	if (!World)
	{
		return "No valid world found";
	}
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (Actor == nullptr) continue;

		// Check if the actor's class matches any of the classes to destroy
		if (Actor->Implements<UMovableActor>())
		{
			Actor->Destroy();
		}
	}
	FString ErrorMessage = SpawnManager->SpawnActorsInLabyrinth(LabyrinthDto);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	OnActorsRespawned.Broadcast();
	return "";
}

void ASceneController::GetPlayerStartPositionAndRotation(FVector& PlayerStartPosition,
                                                         FRotator& PlayerStartRotation) const
{
	PlayerStartPosition = SpawnManager->PlayerStartPosition;
	PlayerStartRotation = SpawnManager->PlayerStartRotation;
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
	
	if (!IsValid(NavMeshBoundsVolume)) return;

	const FVector NewScale {std::size(LabyrinthDto->LabyrinthStructure[0])*WallSettings::WallOffset,
	std::size(LabyrinthDto->LabyrinthStructure)*WallSettings::WallOffset,
	100.f};
	NavMeshBoundsVolume->SetActorScale3D(NewScale);
	NavMeshBoundsVolume->SetActorLocation(FVector{0.f, 0.f, 0.f});
	
	GetWorldTimerManager().SetTimer(NavMeshBoundsTimerHandle, this, &ASceneController::UpdateNavMeshBoundsPosition, 0.25f, false);
}
