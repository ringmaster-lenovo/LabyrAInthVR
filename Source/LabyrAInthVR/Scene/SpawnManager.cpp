﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnManager.h"

#include "Config.h"
#include "SceneController.h"
#include "Utils.h"
#include "CADKernel/UI/Display.h"
#include "LabyrAInthVR/Enemy/RangedEnemy.h"
#include "LabyrAInthVR/Scene/ProceduralSplineWall.h"
#include "LabyrAInthVR/Interagibles/PowerUp.h"
#include "LabyrAInthVR/Interagibles/Trap.h"
#include "LabyrAInthVR/Interagibles/Weapon.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "Logging/StructuredLog.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Labyrinth = nullptr;
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
	}
	// if I have 2 walls around me but not in front of each other, this happens when there is a turn in the labyrinth, I should spawn a trap in this location
	else if (LateralNeighbors == 1 && VerticalNeighbors == 1)
	{
		PotentialTrapSpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
	}
	// else, it's a normal corridor, I could spawn an enemy in this location
	else
	{
		PotentialEnemySpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
	}
}

void ASpawnManager::TriggerFrozenStar()
{
	if(FreezableActors.Num() <= 0) return;
	
	for(const auto& FreezableActor : FreezableActors)
	{
		if(!FreezableActor->Implements<UFreezableActor>()) continue;
		Cast<IFreezableActor>(FreezableActor)->Freeze(10.f);
	}
}

void ASpawnManager::RemoveFromList(AActor* ActorToRemove)
{
	UE_LOG(LogTemp, Error, TEXT("Before: %d %d"), MovableActors.Num(), SpawnedActors.Num())
	
	if (!ActorToRemove->IsA<AProceduralSplineWall>())
		MovableActors.Remove(ActorToRemove);

	SpawnedActors.Remove(ActorToRemove);

	UE_LOG(LogTemp, Error, TEXT("After: %d %d"), MovableActors.Num(), SpawnedActors.Num())
}

/**
 * Spawn the actors in the labyrinth
 * @param LabyrinthDtoReference the labyrinth data transfer object
 * @return an error message describing the blocking error that occured while spawning the actors
 */
FString ASpawnManager::SpawnActorsInLabyrinth(const ULabyrinthDTO* LabyrinthDtoReference)
{
	Labyrinth = NewObject<ULabyrinthDTO>();
	Labyrinth->Height = LabyrinthDtoReference->Height;
	Labyrinth->Width = LabyrinthDtoReference->Width;
	Labyrinth->Complexity = LabyrinthDtoReference->Complexity;
	Labyrinth->Level = LabyrinthDtoReference->Level;
	Labyrinth->LabyrinthStructure = LabyrinthDtoReference->LabyrinthStructure;
	
	NumOfPowerUpsSpawned = 0;
	NumOfTrapsSpawned = 0;
	NumOfEnemiesSpawned = 0;
	NumOfWeaponsSpawned = 0;

	PowerUpsLocations.Empty();
	TrapsLocations.Empty();
	EnemiesLocations.Empty();
	WeaponsLocations.Empty();
	
	FString ErrorMessage = DifficultyDecider();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}

	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Starting to spawn Actors") );
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n%s"), *UUtils::MatrixToString(&Labyrinth->LabyrinthStructure));

	// spawn power-ups first
	PowerUpsLocations.Empty();
	PowerUpsLocations.Reserve(PowerUpsToSpawn);
	ErrorMessage = ChoosePowerUpsSpawnPoints(PowerUpsToSpawn);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	ErrorMessage = SpawnActors(PowerUpsLocations, PowerUpsClasses);
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PowerUpsSpawned:\n%s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfPowerUpsSpawned, &PowerUpsLocations)));
	
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
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("TrapsSpawned:\n%s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfTrapsSpawned, &TrapsLocations)));
	
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
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("EnemiesSpawned:\n%s"), *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfEnemiesSpawned, &EnemiesLocations)));

	ErrorMessage = SpawnWeapons();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
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
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n%s"), *UUtils::MatrixToString(&Labyrinth->LabyrinthStructure));
	
	/*for(const auto& Spawned : SpawnedActors)
	{
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Spawned actor: %s"), *Spawned->GetName());
	}

	for(const auto& Spawned : MovableActors)
	{
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Movable actor: %s"), *Spawned->GetName());
	}*/
	ASceneController* SceneController = Cast<ASceneController>(GetOwner());

	if(!IsValid(SceneController)) return "Error while casting Owner to SceneController";
	
	SceneController->SetSpawnedActors(SpawnedActors);
	SceneController->SetMovableActors(MovableActors);
	SceneController->SetFreezableActors(FreezableActors);
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
	TArray<int> PotentialPowerUpSpawnLocationsCopy = {};
	// Copy elements from the original array to the new array, to avoid modifying the original array
	for (const int32& Element : PotentialPowerUpSpawnLocations)
	{
		PotentialPowerUpSpawnLocationsCopy.Add(Element);
	}
	const int PowerUpsToSpawnFromPotentialLocations = FMath::Min(NumOfPowerUpsToSpawn, PotentialPowerUpSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfPowerUpsToSpawn - PotentialPowerUpSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(PowerUpsToSpawnFromPotentialLocations, PowerUpsLocations, PotentialPowerUpSpawnLocationsCopy, 4);
	// if there are more power-ups to spawn than potential locations, spawn the remaining power-ups using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
	if (DeltaTotalVsPotential > 0)
	{
		TArray<int> PotentialEnemiesSpawnLocationsCopy = {};
		// Copy elements from the original array to the new array, to avoid modifying the original array
		for (const int32& Element : PotentialEnemySpawnLocations)
		{
			PotentialEnemiesSpawnLocationsCopy.Add(Element);
		}
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, PowerUpsLocations, PotentialEnemiesSpawnLocationsCopy, 4);
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
	TArray<int> PotentialTrapsSpawnLocationsCopy = {};
	// Copy elements from the original array to the new array, to avoid modifying the original array
	for (const int32& Element : PotentialTrapSpawnLocations)
	{
		PotentialTrapsSpawnLocationsCopy.Add(Element);
	}
	const int TrapsToSpawnFromPotentialLocations = FMath::Min(NumOfTrapsToSpawn, PotentialTrapSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfTrapsToSpawn - PotentialTrapSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(TrapsToSpawnFromPotentialLocations, TrapsLocations, PotentialTrapsSpawnLocationsCopy, 5);
	// if there are more traps to spawn than potential locations, spawn the remaining traps using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
	if (DeltaTotalVsPotential > 0)
	{
		TArray<int> PotentialEnemiesSpawnLocationsCopy = {};
		// Copy elements from the original array to the new array, to avoid modifying the original array
		for (const int32& Element : PotentialEnemySpawnLocations)
		{
			PotentialEnemiesSpawnLocationsCopy.Add(Element);
		}
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, TrapsLocations, PotentialEnemiesSpawnLocationsCopy, 5);
	}
	NumOfTrapsSpawned = TrapsLocations.Num();
	return ErrorMessage;
}

/**
 * Choose the locations to spawn the enemies
 * It also updates the labyrinth matrix with the enemies locations following the convention of number 6 for enemies
 * @param NumOfEnemiesToSpawn the number of enemies to spawn
 * @return an error message if there are no more free locations to spawn the enemies
 */
FString ASpawnManager::ChooseEnemiesSpawnPoints(const int NumOfEnemiesToSpawn)
{
	TArray<int> PotentialEnemiesSpawnLocationsCopy = {};
	// Copy elements from the original array to the new array, to avoid modifying the original array
	for (const int32& Element : PotentialEnemySpawnLocations)
	{
		PotentialEnemiesSpawnLocationsCopy.Add(Element);
	}
	ChooseRandomSpawnLocation(NumOfEnemiesToSpawn, EnemiesLocations, PotentialEnemiesSpawnLocationsCopy, 6);
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
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PotentialLocations: %d"), PotentialLocations.Num());
	const int LabyrinthSize = Labyrinth->LabyrinthStructure.size() + Labyrinth->LabyrinthStructure[0].size();
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthSize: %d"), LabyrinthSize);
	const int LabyrinthSubdivisions = LabyrinthSize < 50 ? 2 : LabyrinthSize < 200 ? 4 :  LabyrinthSize < 800 ? 8 : 16;  // the bigger the labyrinth, the more subdivisions of the labyrinth, to spawn enemies homogeneously in the labyrinth
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthSubdivisions: %d"), LabyrinthSubdivisions);
	uint8 Subdivision = 0;
	int NumOfActorsSpawned = 0;
	int SpawnLocation = -1;
	while (NumOfActorsSpawned < NumOfActorsToSpawn)
	{
		const int PotentialActorLocationSize = PotentialLocations.Num();
		const int Min = PotentialActorLocationSize / LabyrinthSubdivisions * Subdivision;
		// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Min: %d"), Min);
		const int Max = FMath::Max(Min, (PotentialActorLocationSize / LabyrinthSubdivisions * (Subdivision + 1)) - 1);
		// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Max: %d"), Max);
		
		const int Index = FMath::RandRange(Min, Max);
		// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Index: %d"), Index);
		SpawnLocation = PotentialLocations[Index];
		// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnLocation: %d"), SpawnLocation);
		
		PotentialLocations.RemoveAt(Index);
		ActorsSpawnLocations.Add(SpawnLocation);
		
		int Row = -1;
		int Column = -1;
		UUtils::ConvertToRowColumn(SpawnLocation, Row, Column);
		
		NumOfActorsSpawned++;
		Subdivision = (Subdivision + 1) % LabyrinthSubdivisions;
		Labyrinth->LabyrinthStructure[Row][Column] = ConventionalValueInTheMatrix;
	}
	return "";
}

/**
 * Decide the difficulty of the labyrinth. It is determined by the level of the labyrinth.
 * The difficulty decides the number of power-ups, traps and enemies to spawn.
 * The power-ups and traps are always equal in number and increment by 1 every 5 levels.
 * The enemies increment by 1 every 10 levels.
 * @return an error message if the labyrinth data transfer object is null
 */
FString ASpawnManager::DifficultyDecider()
{
	if (Labyrinth == nullptr) return "Labyrinth is null";
	
	const int Level = Labyrinth->Level;
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Difficulty Decider: Labyrinth Level= %d"), Level);
	
	PowerUpsToSpawn = FMath::Floor(Level / 1);
	TrapsToSpawn = PowerUpsToSpawn;
	EnemiesToSpawn = FMath::Floor(Level / 3) + 1;

	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Difficulty Decider: PowerUpsToSpawn= %d, TrapsToSpawn= %d, EnemiesToSpawn= %d"), PowerUpsToSpawn, TrapsToSpawn, EnemiesToSpawn);
	
	return "";
}

/**
 * Spawn the actors in the labyrinth
 * @param SpawnLocations the locations to spawn the actors
 * @param SpawnableActors the actors to spawn
 * @return an error message describing the blocking error that occured while spawning the actors
 */
FString ASpawnManager::SpawnActors(const TArray<int>& SpawnLocations, const TArray<TSubclassOf<AActor>>& SpawnableActors)
{
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnLocations: %d"), SpawnLocations.Num());
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnableActors: %d"), SpawnableActors.Num());
	if (SpawnableActors.Num() == 0) return "No actors to spawn";
	FVector SpawnPoint{0};
	for (int i = 0; i < SpawnLocations.Num(); i++)
	{
		int Row = -1;
		int Column = -1;
		UUtils::ConvertToRowColumn(SpawnLocations[i], Row, Column);
		
		const int Index = i % SpawnableActors.Num();  // go through the actors to spawn in a round-robin fashion
		const UClass* ObjectClass = SpawnableActors[Index]->GetSuperClass();
		
		if (ObjectClass == APowerUp::StaticClass())
		{
			SpawnPoint = FVector {
				WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
				Interactables::PowerUpsSpawnHeight
			};
		}
		else if (ObjectClass == ATrap::StaticClass())
		{
			double InX = WallSettings::WallOffset * Column;
			double InY = WallSettings::WallOffset * Row;
			for (int j = -1; j <= 1; j += 2)
			{
				for (int k = -1; k <= 1; k += 2)
				{
					if (Labyrinth->LabyrinthStructure[Row + j][Column + k] == 1)
					{
						if (Labyrinth->LabyrinthStructure[Row + j][Column] != 1 || Labyrinth->LabyrinthStructure[Row][Column + k] != 1)
						{
							InX = WallSettings::WallOffset * (Column + k);
							InY = WallSettings::WallOffset * (Row + j);
							break;
						}
					}
				}
			}
			SpawnPoint = FVector { InX, InY,Interactables::TrapsSpawnHeight };
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
		{
			if(ActorSpawned->Implements<UFreezableActor>()) FreezableActors.Add(ActorSpawned);
			
			if (ObjectClass == ARangedEnemy::StaticClass() || ObjectClass == AMeleeEnemy::StaticClass())
			{
				ABaseEnemy* EnemyInstance = Cast<ABaseEnemy>(ActorSpawned);
				if (EnemyInstance == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("EnemyInstance is null"))
				EnemyInstance->SetLabyrinthMatrix(Labyrinth);
				EnemyInstance->SetMatrixPosition(Row, Column);
			}

			SpawnedActors.Add(ActorSpawned);
			MovableActors.Add(ActorSpawned);
		}
			UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor spawned: %s"), *ActorSpawned->GetName());
	}
	return "";
}

FString ASpawnManager::SpawnWeapons()
{
	if (WeaponsClasses.IsEmpty()) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Non blocking scene Error: No Weapons Set"));
	if (PickupsClasses.IsEmpty()) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Non blocking scene Error: No Pickups Set"));
	if (Labyrinth == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Blocking scene Error: Labyrinth is null"));
	if (PlayerStartIndexPosition == -1)
	{
		UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Did not found the player start position, invalid matrix"));
		return "";
	}
	int Row = -1;
	int Column = -1;
	UUtils::ConvertToRowColumn(PlayerStartIndexPosition, Row, Column);
	FVector SpawnPoint{0};
	FVector PickupsSpawnPoint{0};
	
	double InX = WallSettings::WallOffset * Column;
	double InY = WallSettings::WallOffset * Row;
	for (int j = -1; j <= 1; j += 2)
	{
		for (int k = -1; k <= 1; k += 2)
		{
			if (Labyrinth->LabyrinthStructure[Row][Column + k] == 1)
			{
				InX = WallSettings::WallOffset * (Column + k) - (k * Weapons::DistanceFromWall);
				InY = WallSettings::WallOffset * (Row);
				break;
			}
			else if (Labyrinth->LabyrinthStructure[Row + j][Column] == 1)
			{
				InX = WallSettings::WallOffset * (Column);
				InY = WallSettings::WallOffset * (Row + j) - (j * Weapons::DistanceFromWall);
				break;
			}
		}
	}
	SpawnPoint = FVector { InX, InY,Weapons::SpawnHeight };
	PickupsSpawnPoint = FVector { 170, 20,140 };
	const UClass* ObjectClass = WeaponsClasses[0]->GetSuperClass();
	AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(WeaponsClasses[0], SpawnPoint, FRotator(0, 0, 0));
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	else
	{
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor spawned: %s"), *ActorSpawned->GetName());
		SpawnedActors.Add(ActorSpawned);
		MovableActors.Add(ActorSpawned);
	}
	AActor* AnotherActorSpawned = GetWorld()->SpawnActor<AActor>(PickupsClasses[0], PickupsSpawnPoint, FRotator(0, 90, 0));
	if (AnotherActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	else
	{
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor spawned: %s"), *AnotherActorSpawned->GetName());
		SpawnedActors.Add(AnotherActorSpawned);
		MovableActors.Add(AnotherActorSpawned);
	}
	return "";
}


FString ASpawnManager::SpawnPortal()
{
	if (PortalIndexPosition == -1) return "Did not found the portal position, invalid matrix";
	if (Portal == nullptr) return "Portal asset not set, cannot play";
	int Row = -1;
	int Column = -1;
	UUtils::ConvertToRowColumn(PortalIndexPosition, Row, Column);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Portal Position = Row: %d  Column: %d"), Row, Column);
	const FVector SpawnPoint = FVector {
		WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
		140.0
	};
	double YawRotation = 90;
	if (Row + 1 < Labyrinth->LabyrinthStructure.size() && Row - 1 >= 0)
	{
		if (Labyrinth->LabyrinthStructure[Row + 1][Column] != 0 && Labyrinth->LabyrinthStructure[Row - 1][Column] != 0)
		{
			YawRotation = 0;
		}
	}
	AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(Portal, SpawnPoint, FRotator(0, YawRotation, 0));
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	SpawnedActors.Add(ActorSpawned);
	MovableActors.Add(ActorSpawned);
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
	PlayerStartPosition = {SpawnPoint.X, SpawnPoint.Y, SpawnPoint.Z + 110};
	PlayerStartRotation = {0, 0, 0};
	AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(PlayerSpawnPoint, SpawnPoint, FRotator(0, 0, 0));
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	MovableActors.Add(ActorSpawned);
	SpawnedActors.Add(ActorSpawned);
	return "";
}

void ASpawnManager::GetNumOfActorSpawned(int &NumOfEnemies, int &NumOfTraps, int &NumOfPowerUps, int &NumOfWeapons) const
{
	NumOfEnemies = NumOfEnemiesSpawned;
	NumOfTraps = NumOfTrapsSpawned;
	NumOfPowerUps = NumOfPowerUpsSpawned;
	NumOfWeapons = NumOfWeaponsSpawned;
}

void ASpawnManager::UpdateSpawnableActor(AActor* SpawnedWall)
{
	SpawnedActors.Add(SpawnedWall);
}
