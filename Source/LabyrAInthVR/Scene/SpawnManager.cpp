// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnManager.h"

#include "SceneController.h"
#include "Utils.h"
#include "LabyrAInthVR/Network/LabyrinthDTO.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LabyrinthDTO = nullptr;
}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/**
 * Find the potential spawn locations for the power-ups, traps and enemies
 * Invoked by LabyrinthParser while traversing the labyrinth matrix horizontally
 * @param LabyrinthDTOObject the labyrinth data transfer object
 * @param Row the row of the labyrinth matrix currently being traversed
 * @param Column the column of the labyrinth matrix currently being traversed
 */
void ASpawnManager::FindPotentialSpawnLocations(const ULabyrinthDTO* LabyrinthDTOObject, const int Row, const int Column)
{
	if (Row == 0 || Row == std::size(LabyrinthDTOObject->LabyrinthStructure) - 1 || Column == 0 || Column == std::size(LabyrinthDTOObject->LabyrinthStructure[0]) - 1)
	{
		// should never enter here if we have a 1 in the border, hence outer walls
		return;
	}
	const uint8 LateralNeighbors = LabyrinthDTOObject->LabyrinthStructure[Row][Column - 1] + LabyrinthDTOObject->LabyrinthStructure[Row][Column + 1];
	const uint8 VerticalNeighbors = LabyrinthDTOObject->LabyrinthStructure[Row - 1][Column] + LabyrinthDTOObject->LabyrinthStructure[Row + 1][Column];

	// the concept is that if I have 3 walls around me, it is the end of a corridor, I should spawn a power up in this location
	if (LateralNeighbors + VerticalNeighbors == 3)
	{
		PotentialPowerUpSpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Potential power up location: %d : %d"), Row, Column);
	}
	// if I have 2 walls around me but not in front of each other, this happens when there is a turn in the labyrinth, I should spawn a trap in this location
	else if (LateralNeighbors == 1 && VerticalNeighbors == 1)
	{
		PotentialTrapSpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Potential trap location: %d : %d"), Row, Column);
	}
	// else, its a normal corridor, I could spawn an enemy in this location
	else
	{
		PotentialEnemySpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Potential enemy location: %d : %d"), Row, Column);
	}
}

/**
 * Spawn the actors in the labyrinth
 * @param LabyrinthDTOReference the labyrinth data transfer object
 * @return an error message describing the blocking error that occured while spawning the actors
 */
FString ASpawnManager::SpawnActorsInLabyrinth(ULabyrinthDTO* LabyrinthDTOReference)
{
	LabyrinthDTO = LabyrinthDTOReference;
	SetDifficultyLevel(LabyrinthDTOReference->Level);
	
	// start a timer to measure the time it takes to spawn the actors
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, 1.0f, false);

	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));

	// spawn power-ups first
	PowerUpsLocations.Empty();
	PowerUpsLocations.Reserve(5);
	FString ErrorMessage = "";
	ErrorMessage = ChoosePowerUpsSpawnPoints(3);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PowerUpsSpawned:\n %s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfPowerUpsSpawned, &PowerUpsLocations)));
	
	// spawn traps second
	TrapsLocations.Empty();
	TrapsLocations.Reserve(5);
	ErrorMessage = ChooseTrapsSpawnPoints(3);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("TrapsSpawned:\n %s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfTrapsSpawned, &TrapsLocations)));
	
	// spawn enemies last
	EnemiesLocations.Empty();
	EnemiesLocations.Reserve(5);
	ErrorMessage = ChooseEnemiesSpawnPoints(5);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("EnemiesSpawned:\n %s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfEnemiesSpawned, &EnemiesLocations)));
	return "";
}

/**
 * Choose the locations to spawn the power-ups
 * It also updates the labyrinth matrix with the power-ups locations following the convention of number 4 for power-ups
 * @param NumOfPowerUpsToSpawn the number of power-ups to spawn
 * @return an error message if there are no more free locations to spawn the power-ups
 */
FString ASpawnManager::ChoosePowerUpsSpawnPoints(const int NumOfPowerUpsToSpawn)
{
	FString ErrorMessage = "";
	const int TrapsToSpawnFromPotentialLocations = FMath::Min(NumOfPowerUpsToSpawn, PotentialPowerUpSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfPowerUpsToSpawn - PotentialPowerUpSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(TrapsToSpawnFromPotentialLocations, PowerUpsLocations, PotentialPowerUpSpawnLocations, 4);
	if (DeltaTotalVsPotential > 0)
	{
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, PowerUpsLocations, PotentialEnemySpawnLocations, 4);
	}
	NumOfPowerUpsSpawned = PowerUpsLocations.Num();
	return ErrorMessage;
}

/**
 * Choose the locations to spawn the traps
 * It also updates the labyrinth matrix with the traps locations following the convention of number 5 for traps
 * @param NumOfTrapsToSpawn the number of traps to spawn
 * @return an error message if there are no more free locations to spawn the traps
 */
FString ASpawnManager::ChooseTrapsSpawnPoints(const int NumOfTrapsToSpawn)
{
	FString ErrorMessage = "";
	const int TrapsToSpawnFromPotentialLocations = FMath::Min(NumOfTrapsToSpawn, PotentialTrapSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfTrapsToSpawn - PotentialTrapSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(TrapsToSpawnFromPotentialLocations, TrapsLocations, PotentialTrapSpawnLocations, 5);
	if (DeltaTotalVsPotential > 0)
	{
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, TrapsLocations, PotentialEnemySpawnLocations, 5);
	}
	NumOfTrapsSpawned = TrapsLocations.Num();
	return ErrorMessage;
}

/**
 * Choose the locations to spawn the enemies
 * It also updates the labyrinth matrix with the enemies locations following the convention of number 6 for enemies
 * @param NumOfeEnemiesToSpawn the number of enemies to spawn
 * @return an error message if there are no more free locations to spawn the enemies
 */
FString ASpawnManager::ChooseEnemiesSpawnPoints(const int NumOfeEnemiesToSpawn)
{
	ChooseRandomSpawnLocation(NumOfeEnemiesToSpawn, EnemiesLocations, PotentialEnemySpawnLocations, 6);
	NumOfEnemiesSpawned = EnemiesLocations.Num();
	return "";
}

FString ASpawnManager::ChooseRandomSpawnLocation(const int NumOfActorsToSpawn, TSet<int>& ActorsSpawnLocations, TArray<int>& PotentialLocations, const uint8 ConventionalValueInTheMatrix) const
{
	if (NumOfActorsToSpawn > PotentialLocations.Num()) return "No free location to spawn actors";
	if (ConventionalValueInTheMatrix <= 3) return "Invalid conventional value in the matrix";
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PotentialLocations: %d"), PotentialLocations.Num());
	const int LabyrinthSize = LabyrinthDTO->LabyrinthStructure.size() + LabyrinthDTO->LabyrinthStructure[0].size();
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthSize: %d"), LabyrinthSize);
	const int LabyrinthSubdivisions = LabyrinthSize < 50 ? 2 : LabyrinthSize < 200 ? 4 :  LabyrinthSize < 800 ? 8 : 16;  // the bigger the labyrinth, the more subdivisions of the labyrinth, to spawn enemies homogeneously in the labyrinth
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthSubdivisions: %d"), LabyrinthSubdivisions);
	uint8 Subdivision = 0;
	int NumOfActorsSpawned = 0;
	int SpawnLocation = -1;
	while (NumOfActorsSpawned < NumOfActorsToSpawn)
	{
		const int PotentialActorLocationSize = PotentialLocations.Num();
		const int Min = PotentialActorLocationSize / LabyrinthSubdivisions * Subdivision;
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Min: %d"), Min);
		const int Max = FMath::Max(Min, (PotentialActorLocationSize / LabyrinthSubdivisions * (Subdivision + 1)) - 1);
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Max: %d"), Max);
		
		const int Index = FMath::RandRange(Min, Max);
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Index: %d"), Index);
		SpawnLocation = PotentialLocations[Index];
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnLocation: %d"), SpawnLocation);
		
		PotentialLocations.RemoveAt(Index);
		ActorsSpawnLocations.Add(SpawnLocation);
		
		int Row = -1;
		int Column = -1;
		UUtils::ConvertToRowColumn(SpawnLocation, Row, Column);
		
		NumOfActorsSpawned++;
		Subdivision = (Subdivision + 1) % LabyrinthSubdivisions;
		LabyrinthDTO->LabyrinthStructure[Row][Column] = ConventionalValueInTheMatrix;
	}
	return "";
}

// TODO: Implement SpawnActors
FString ASpawnManager::SpawnActors(const TSet<int>& SpawnLocations,
	const TMap<TSubclassOf<AActor>, int>& ActorsToSpawn)
{
	return "";
}