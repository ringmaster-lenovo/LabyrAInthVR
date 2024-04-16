// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnManager.h"

#include "Config.h"
#include "SceneController.h"
#include "Utils.h"
#include "LabyrAInthVR/Enemy/RangedEnemy.h"
#include "LabyrAInthVR/Interagibles/PowerUp.h"
#include "LabyrAInthVR/Interagibles/Trap.h"
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
	if (LabyrinthDTOObject->LabyrinthStructure[Row][Column] == 2)
	{
		PlayerStartIndexPosition = UUtils::ConvertToIndex(Row, Column);
		return;
	}
	if (LabyrinthDTOObject->LabyrinthStructure[Row][Column] == 3)
	{
		PortalIndexPosition = UUtils::ConvertToIndex(Row, Column);
		return;
	}
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
	
	// start a timer to measure the time it takes to spawn the actors
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, 1.0f, false);

	int PowerUpsToSpawn = 0;
	int TrapsToSpawn = 0;
	int EnemiesToSpawn = 0;
	FString ErrorMessage = DifficultyDecider(PowerUpsToSpawn, TrapsToSpawn, EnemiesToSpawn);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}

	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));

	// spawn power-ups first
	PowerUpsLocations.Empty();
	PowerUpsLocations.Reserve(PowerUpsToSpawn);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PowerUpsLocations: %d"), PowerUpsLocations.Num());
	ErrorMessage = ChoosePowerUpsSpawnPoints(PowerUpsToSpawn);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PowerUpsLocations: %d"), PowerUpsLocations.Num());
	ErrorMessage = SpawnActors(PowerUpsLocations, PowerUpsClasses);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PowerUpsSpawned:\n %s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfPowerUpsSpawned, &PowerUpsLocations)));
	
	// spawn traps second
	TrapsLocations.Empty();
	TrapsLocations.Reserve(TrapsToSpawn);
	ErrorMessage = ChooseTrapsSpawnPoints(TrapsToSpawn);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	ErrorMessage = SpawnActors(TrapsLocations, TrapsClasses);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("TrapsSpawned:\n %s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfTrapsSpawned, &TrapsLocations)));
	
	// spawn enemies last
	EnemiesLocations.Empty();
	EnemiesLocations.Reserve(EnemiesToSpawn);
	ErrorMessage = ChooseEnemiesSpawnPoints(EnemiesToSpawn);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	ErrorMessage = SpawnActors(EnemiesLocations, EnemiesClasses);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n %s"), *UUtils::MatrixToString(&LabyrinthDTO->LabyrinthStructure));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("EnemiesSpawned:\n %s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfEnemiesSpawned, &EnemiesLocations)));

	ErrorMessage = SpawnPortal();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	ErrorMessage = SpawnPlayerStart();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
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
	const int PowerUpsToSpawnFromPotentialLocations = FMath::Min(NumOfPowerUpsToSpawn, PotentialPowerUpSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfPowerUpsToSpawn - PotentialPowerUpSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(PowerUpsToSpawnFromPotentialLocations, PowerUpsLocations, PotentialPowerUpSpawnLocations, 4);
	// it there are more power-ups to spawn than potential locations, spawn the remaining power-ups using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
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
	// it there are more traps to spawn than potential locations, spawn the remaining traps using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
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

/**
 * Choose a random location to spawn actors
 * the algorithm is as follows:
 * the labyrinth is divided into subdivisions, the number of subdivisions is determined by the size of the labyrinth
 * it divides the potential location array into sub parts of equal length, it then chooses a random index from the sub part
 * and removes it from the potential locations array, adding the location at that index to the actors spawn locations
 * this to ensure that the actors are spawned homogeneously in the labyrinth
 * @param NumOfActorsToSpawn the number of actors to spawn
 * @param ActorsSpawnLocations the locations of the actors spawned, will be updated with the new locations
 * @param PotentialLocations the potential locations to spawn the actors
 * @param ConventionalValueInTheMatrix the conventional value in the matrix to update the labyrinth matrix with
 * @return an error message if there are no more free locations to spawn the actors
 */
FString ASpawnManager::ChooseRandomSpawnLocation(const int NumOfActorsToSpawn, TArray<int>& ActorsSpawnLocations, TArray<int>& PotentialLocations, const uint8 ConventionalValueInTheMatrix) const
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

/**
 * Decide the difficulty of the labyrinth. It is determined by the level of the labyrinth.
 * The difficulty decides the number of power-ups, traps and enemies to spawn.
 * The power-ups and traps are always equal in number and increment by 1 every 5 levels.
 * The enemies increment by 1 every 10 levels.
 * @param PowerUpsToSpawn the number of power-ups to spawn
 * @param TrapsToSpawn the number of traps to spawn
 * @param EnemiesToSpawn the number of enemies to spawn
 * @return an error message if the labyrinth data transfer object is null
 */
FString ASpawnManager::DifficultyDecider(int& PowerUpsToSpawn, int& TrapsToSpawn, int& EnemiesToSpawn) const
{
	if (LabyrinthDTO == nullptr) return "LabyrinthDTO is null";
	
	const int Level = LabyrinthDTO->Level;
	
	PowerUpsToSpawn = FMath::Floor(Level / 5);
	TrapsToSpawn = PowerUpsToSpawn;
	EnemiesToSpawn = FMath::Floor(Level / 10) + 1;
	
	return "";
}


FString ASpawnManager::SpawnActors(const TArray<int>& SpawnLocations, const TArray<TSubclassOf<AActor>>& SpawnableActors) const
{
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnLocations: %d"), SpawnLocations.Num());
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnableActors: %d"), SpawnableActors.Num());
	if (SpawnableActors.Num() == 0) return "No actors to spawn";
	FVector SpawnPoint{0};
	for (int i = 0; i < SpawnLocations.Num(); i++)
	{
		int Row = -1;
		int Column = -1;
		UUtils::ConvertToRowColumn(SpawnLocations[i], Row, Column);
		
		const int Index = i % SpawnableActors.Num();  // go trough the actors to spawn in a round robin fashion
		const UClass* ObjectClass = SpawnableActors[Index]->GetSuperClass();
		
		if (ObjectClass == APowerUp::StaticClass())
		{
			SpawnPoint = FVector {
				WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
				Interagibles::PowerUpsSpawnHeight
			};
		}
		else if (ObjectClass == ATrap::StaticClass())
		{
			SpawnPoint = FVector {
				WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
				Interagibles::TrapsSpawnHeight
			};
		}
		else
		{
			SpawnPoint = FVector {
				WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
				EnemySettings::SpawnHeight
			};
		}

		AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(SpawnableActors[Index], SpawnPoint, FRotator(0, FMath::RandRange(0, 360), 0));
		if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
		else
			if (ObjectClass == ARangedEnemy::StaticClass() || ObjectClass == AMeleeEnemy::StaticClass())
			{
				ABaseEnemy* EnemyInstance = Cast<ABaseEnemy>(ActorSpawned);
				if (EnemyInstance == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("EnemyInstance is null"))
				EnemyInstance->SetLabyrinthMatrix(LabyrinthDTO);
				EnemyInstance->SetMatrixPosition(Row, Column);
			}
			UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor spawned: %s"), *ActorSpawned->GetName());
			UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor class: %s"), *ObjectClass->GetSuperClass()->GetName());
			UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor class: %s"), *ObjectClass->GetName());
	}
	return "";
}

FString ASpawnManager::SpawnPortal() const
{
	if (PortalIndexPosition == -1) return "Did not found the portal position, invalid matrix";
	if (Portal == nullptr) return "Portal asset not set, cannot play";
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT(""));
	int Row = -1;
	int Column = -1;
	UUtils::ConvertToRowColumn(PortalIndexPosition, Row, Column);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Portal Position = Row: %d  Column: %d"), Row, Column);
	const FVector SpawnPoint = FVector {
		WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
		140.0
	};
	const AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(Portal, SpawnPoint, FRotator(0, 90, 0));
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	return "";
}

FString ASpawnManager::SpawnPlayerStart()
{
	if (PlayerStartIndexPosition == -1) return "Did not found the player start position, invalid matrix";
	if (PlayerSpawnPoint == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Non blocking scene Error: PlayerSpawnPoint BP not set"));
	int Row = -1;
	int Column = -1;
	UUtils::ConvertToRowColumn(PlayerStartIndexPosition, Row, Column);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Player Start Position = Row: %d  Column: %d"), Row, Column);
	const FVector SpawnPoint = FVector {
		WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
		0.0
	};
	PlayerStartPosition = {SpawnPoint.X, SpawnPoint.Y, SpawnPoint.Z + 150};
	PlayerStartRotation = {0, 45, 0};
	const AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(PlayerSpawnPoint, SpawnPoint, FRotator(0, 0, 0));
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	return "";
}
