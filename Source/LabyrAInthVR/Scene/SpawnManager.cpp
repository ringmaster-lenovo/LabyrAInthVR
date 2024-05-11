// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnManager.h"

#include "Config.h"
#include "SceneController.h"
#include "Utils.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/VRGameMode.h"
#include "LabyrAInthVR/Enemy/RangedEnemy.h"
#include "LabyrAInthVR/Interagibles/Portal.h"
#include "LabyrAInthVR/Scene/ProceduralSplineWall.h"
#include "LabyrAInthVR/Interagibles/PowerUp.h"
#include "LabyrAInthVR/Interagibles/Trap.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "LabyrAInthVR/Pickups/BasePickup.h"

class AVRGameMode;
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

	if (!IsValid(CompassInstance) || !IsValid(MainCharacter)) return;
	FVector ToPortal = (EndPortalPosition - MainCharacter->GetActorLocation()).GetSafeNormal() * 500.f;
	CompassInstance->SetWorldLocation(MainCharacter->GetActorLocation());
	CompassInstance->SetWorldRotation(ToPortal.Rotation());
}

/**
 * Find the potential spawn locations for the power-ups, traps and enemies
 * Invoked by LabyrinthParser while traversing the labyrinth matrix horizontally
 * @param LabyrinthDTOObject the labyrinth data transfer object
 * @param Row the row of the labyrinth matrix currently being traversed
 * @param Column the column of the labyrinth matrix currently being traversed
 */
void ASpawnManager::FindPotentialSpawnLocations(const ULabyrinthDTO* LabyrinthDTOObject, const int Row,
                                                const int Column)
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
	if (Row == 0 || Row == std::size(LabyrinthDTOObject->LabyrinthStructure) - 1 || Column == 0 || Column == std::size(
		LabyrinthDTOObject->LabyrinthStructure[0]) - 1)
	{
		// should never enter here if we have a 1 in the border, hence outer walls
		return;
	}
	const uint8 LateralNeighbors = LabyrinthDTOObject->LabyrinthStructure[Row][Column - 1] + LabyrinthDTOObject->
		LabyrinthStructure[Row][Column + 1];
	const uint8 VerticalNeighbors = LabyrinthDTOObject->LabyrinthStructure[Row - 1][Column] + LabyrinthDTOObject->
		LabyrinthStructure[Row + 1][Column];

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
	// if I have 2 walls around me, and they are in front of each other, this happens when there is a corridor, I should spawn a weapon in this location
	else if (LateralNeighbors == 2 || VerticalNeighbors == 2)
	{
		PotentialWeaponSpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
	}
	// else, it's a normal corridor, I could spawn an enemy in this location
	else
	{
		PotentialEnemySpawnLocations.Add(UUtils::ConvertToIndex(Row, Column));
	}
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
	FreezableActors.Empty();

	FString ErrorMessage = DifficultyDecider();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}

	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Starting to spawn Actors"));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n%s"),
	       *UUtils::MatrixToString(&Labyrinth->LabyrinthStructure));

	// create a copy of the potential enemy spawn locations to avoid modifying the original array
	// so that the spawners can modify the copy of the potential enemy spawn locations
	PotentialEnemySpawnLocationsCopy.Empty();
	PotentialEnemySpawnLocationsCopy.Reserve(PotentialEnemySpawnLocations.Num());
	for (const int32& Element : PotentialEnemySpawnLocations)
	{
		PotentialEnemySpawnLocationsCopy.Add(Element);
	}

	PotentialTrapSpawnLocationsCopy.Empty();
	PotentialTrapSpawnLocationsCopy.Reserve(PotentialTrapSpawnLocations.Num());
	for (const int32& Element : PotentialTrapSpawnLocations)
	{
		PotentialTrapSpawnLocationsCopy.Add(Element);
	}

	PotentialPowerUpSpawnLocationsCopy.Empty();
	PotentialPowerUpSpawnLocationsCopy.Reserve(PotentialPowerUpSpawnLocations.Num());
	for (const int32& Element : PotentialPowerUpSpawnLocations)
	{
		PotentialPowerUpSpawnLocationsCopy.Add(Element);
	}

	PotentialWeaponSpawnLocationsCopy.Empty();
	PotentialWeaponSpawnLocationsCopy.Reserve(PotentialWeaponSpawnLocations.Num());
	for (const int32& Element : PotentialWeaponSpawnLocations)
	{
		PotentialWeaponSpawnLocationsCopy.Add(Element);
	}

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
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PowerUpsSpawned:\n%s"),
	       *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfPowerUpsSpawned, &PowerUpsLocations)));

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
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("TrapsSpawned:\n%s"),
	       *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfTrapsSpawned, &TrapsLocations)));

	// spawn enemies third
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
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("EnemiesSpawned:\n%s"),
	       *UUtils::StructToString(UUtils::GetInfoActorSpawned(NumOfEnemiesSpawned, &EnemiesLocations)));

	// spawn weapons last
	WeaponsLocations.Empty();
	WeaponsLocations.Reserve(WeaponsToSpawn);
	ErrorMessage = ChooseWeaponsSpawnPoints(WeaponsToSpawn - 1);
	// 1 weapon is always spawned at the player start location
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	ErrorMessage = SpawnWeapons();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	// spawn the end portal of the labyrinth
	ErrorMessage = SpawnPortal();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	// spawn the spawn portal of the player, its starting point in the labyrinth
	ErrorMessage = SpawnPlayerStart();
	if (ErrorMessage != "")
	{
		return ErrorMessage;
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthMatrix:\n%s"),
	       *UUtils::MatrixToString(&Labyrinth->LabyrinthStructure));

	ASceneController* SceneController = Cast<ASceneController>(GetOwner());

	if (!IsValid(SceneController)) return "Error while casting Owner to SceneController";

	SceneController->SetSpawnedActors(SpawnedActors);
	SceneController->SetMovableActors(MovableActors);
	SceneController->SetFreezableActors(FreezableActors);
	return "";
}

FString ASpawnManager::SpawnActorsInDemoLabyrinth()
{
	if (PowerUpsClasses.Num() == 0 || TrapsClasses.Num() == 0 || EnemiesClasses.Num() == 0 || WeaponsClasses.Num() == 0
		|| PickupsClasses.Num() == 0 || PlayerSpawnPoint == nullptr || Portal == nullptr)
	{
		return "No actors to spawn";
	}
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* PlayerSpawn = GetWorld()->SpawnActor<AActor>(PlayerSpawnPoint, FVector{-1570,-4550,15}, FRotator{0,0,0}, SpawnParams);
	AActor* Weapon = GetWorld()->SpawnActor<AActor>(WeaponsClasses[0], FVector{-1720,-4440,140}, FRotator{0,0,0}, SpawnParams);
	AActor* Pickup = GetWorld()->SpawnActor<AActor>(PickupsClasses[0], FVector{-1430,-4440,140}, FRotator{0,0,0}, SpawnParams);
	AActor* Enemy = GetWorld()->SpawnActor<AActor>(EnemiesClasses[0], FVector{-1550,-4160,77.5}, FRotator{0,90,0}, SpawnParams);
	ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(Enemy);
	if (BaseEnemy != nullptr)
	{
		BaseEnemy->SetHealth(1);
	}
	AActor* PowerUp = GetWorld()->SpawnActor<AActor>(PowerUpsClasses[0], FVector{-1575,-3860,15}, FRotator{0,0,0}, SpawnParams);
	AActor* Trap = GetWorld()->SpawnActor<AActor>(TrapsClasses[0], FVector{-1574,-3538,-15}, FRotator{0,0,0}, SpawnParams);
	AActor* PortalSpawn = GetWorld()->SpawnActor<AActor>(Portal, FVector{-1590,-2900,155}, FRotator{0,90,0}, SpawnParams);
	EndPortalPosition = PortalSpawn->GetActorLocation();
	EndPortalRotation = PortalSpawn->GetActorRotation();

	SpawnedActors.Add(PlayerSpawn);
	SpawnedActors.Add(Weapon);
	SpawnedActors.Add(Pickup);
	SpawnedActors.Add(Enemy);
	SpawnedActors.Add(PowerUp);
	SpawnedActors.Add(Trap);
	SpawnedActors.Add(PortalSpawn);
	
	ASceneController* SceneController = Cast<ASceneController>(GetOwner());
	if (!IsValid(SceneController)) return "Error while casting Owner to SceneController";
	SceneController->SetSpawnedActors(SpawnedActors);
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
	const int PowerUpsToSpawnFromPotentialLocations = FMath::Min(NumOfPowerUpsToSpawn,
	                                                             PotentialPowerUpSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfPowerUpsToSpawn - PotentialPowerUpSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(PowerUpsToSpawnFromPotentialLocations, PowerUpsLocations,
	                                         PotentialPowerUpSpawnLocationsCopy, 4);
	// if there are more power-ups to spawn than potential locations, spawn the remaining power-ups using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
	if (DeltaTotalVsPotential > 0)
	{
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, PowerUpsLocations,
		                                         PotentialEnemySpawnLocationsCopy, 4);
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
	ErrorMessage = ChooseRandomSpawnLocation(TrapsToSpawnFromPotentialLocations, TrapsLocations,
	                                         PotentialTrapSpawnLocationsCopy, 5);
	// if there are more traps to spawn than potential locations, spawn the remaining traps using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
	if (DeltaTotalVsPotential > 0)
	{
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, TrapsLocations,
		                                         PotentialEnemySpawnLocationsCopy, 5);
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
	FString ErrorMessage = ChooseRandomSpawnLocation(NumOfEnemiesToSpawn, EnemiesLocations,
	                                                 PotentialEnemySpawnLocationsCopy, 6);
	NumOfEnemiesSpawned = EnemiesLocations.Num();
	return ErrorMessage;
}


/**
 * Choose the locations to spawn the weapons
 * It also updates the labyrinth matrix with the weapons locations following the convention of number 7 for weapons
 * @param NumOfWeaponsToSpawn the number of weapons to spawn
 * @return an error message if there are no more free locations to spawn the weapons
 */
FString ASpawnManager::ChooseWeaponsSpawnPoints(const int NumOfWeaponsToSpawn)
{
	FString ErrorMessage = "";
	const int WeaponsToSpawnFromPotentialLocations = FMath::Min(NumOfWeaponsToSpawn,
	                                                            PotentialWeaponSpawnLocations.Num());
	const int DeltaTotalVsPotential = NumOfWeaponsToSpawn - PotentialWeaponSpawnLocations.Num();
	ErrorMessage = ChooseRandomSpawnLocation(WeaponsToSpawnFromPotentialLocations, WeaponsLocations,
	                                         PotentialWeaponSpawnLocationsCopy, 7);
	// if there are more traps to spawn than potential locations, spawn the remaining traps using the enemy potential locations, which are filled of every 0 in the labyrinth matrix which is not a power up or trap potential location
	if (DeltaTotalVsPotential > 0)
	{
		ErrorMessage = ChooseRandomSpawnLocation(DeltaTotalVsPotential, WeaponsLocations,
		                                         PotentialEnemySpawnLocationsCopy, 7);
	}
	NumOfWeaponsSpawned = WeaponsLocations.Num();
	return ErrorMessage;
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
FString ASpawnManager::ChooseRandomSpawnLocation(const int NumOfActorsToSpawn, TArray<int>& ActorsSpawnLocations,
                                                 TArray<int>& PotentialLocations,
                                                 const uint8 ConventionalValueInTheMatrix) const
{
	if (NumOfActorsToSpawn > PotentialLocations.Num()) return "No free location to spawn actors";
	if (ConventionalValueInTheMatrix <= 3) return "Invalid conventional value in the matrix";
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("PotentialLocations: %d"), PotentialLocations.Num());
	const int LabyrinthSize = Labyrinth->LabyrinthStructure.size() + Labyrinth->LabyrinthStructure[0].size();
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthSize: %d"), LabyrinthSize);
	const int LabyrinthSubdivisions = LabyrinthSize < 40 ? 2 : LabyrinthSize < 100 ? 4 : LabyrinthSize < 200 ? 8 : 16;
	// the bigger the labyrinth, the more subdivisions of the labyrinth, to spawn enemies homogeneously in the labyrinth
	// UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("LabyrinthSubdivisions: %d"), LabyrinthSubdivisions);
	uint8 Subdivision = 0;
	int NumOfActorsSpawned = 0;
	int SpawnLocation = -1;
	// int NumTries = 0;
	while (NumOfActorsSpawned < NumOfActorsToSpawn)
	{
		const int PotentialActorLocationSize = PotentialLocations.Num();
		const int Min = PotentialActorLocationSize / LabyrinthSubdivisions * Subdivision;
		const int Max = FMath::Max(Min, (PotentialActorLocationSize / LabyrinthSubdivisions * (Subdivision + 1)) - 1);
		
		const int Index = FMath::RandRange(Min, Max);
		// if (Index >= PotentialLocations.Num()) return "Index out of bounds";
		
		// check if Index is inside the radius of 2 of the player start position
		// if (PlayerStartIndexPosition != -1)
		// {
			// int PlayerStartRow = -1;
			// int PlayerStartColumn = -1;
			// UUtils::ConvertToRowColumn(PlayerStartIndexPosition, PlayerStartRow, PlayerStartColumn);
			// if (FMath::Abs(Row - PlayerStartRow) <= 1 && FMath::Abs(Column - PlayerStartColumn) <= 1)
			// {
				// NumTries++;
				// if (NumTries < 10) continue;
			// }
			// NumTries = 0;
		// }
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Index: %d"), Index);
		if (Index >= PotentialLocations.Num() || Index < 0) return "Index out of bounds";
		SpawnLocation = PotentialLocations[Index];
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("SpawnLocation: %d"), SpawnLocation);
		
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

	PowerUpsToSpawn = FMath::Floor(3 * Level / 2);
	TrapsToSpawn = PowerUpsToSpawn;
	EnemiesToSpawn = FMath::Floor(2 * Level / 3) + 1;
	
	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->IsInVR())
	{
		WeaponsToSpawn = FMath::Floor(3 * EnemiesToSpawn / 2);
	}
	else
	{
		WeaponsToSpawn = EnemiesToSpawn;
	}

	UE_LOG(LabyrAInthVR_Scene_Log, Display,
	       TEXT("Difficulty Decider: PowerUpsToSpawn= %d, TrapsToSpawn= %d, EnemiesToSpawn= %d, WeaponsToSpawn=%d"),
	       PowerUpsToSpawn, TrapsToSpawn, EnemiesToSpawn, WeaponsToSpawn);

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
	if (SpawnableActors.Num() == 0) return "No actors to spawn";

	FVector SpawnPoint{0};
	FRotator SpawnRotation = {0, FMath::RandRange(0.0f, 360.0f), 0};
	for (int i = 0; i < SpawnLocations.Num(); i++)
	{
		int Row = -1;
		int Column = -1;
		UUtils::ConvertToRowColumn(SpawnLocations[i], Row, Column);

		const int Index = i % SpawnableActors.Num(); // go through the actors to spawn in a round-robin fashion
		if (Index >= SpawnableActors.Num()) return "Index out of bounds";
		if (SpawnableActors[Index] == nullptr) return "";
		const UClass* ObjectClass = SpawnableActors[Index]->GetSuperClass();

		if (ObjectClass == APowerUp::StaticClass())
		{
			SpawnPoint = FVector{
				WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
				Interactables::PowerUpsSpawnHeight
			};
		}
		else if (ObjectClass == ATrap::StaticClass())
		{
			ATrap* TrapInstance = SpawnableActors[Index]->GetDefaultObject<ATrap>();
			if (TrapInstance == nullptr) return "TrapInstance is null";
			double InX = WallSettings::WallOffset * Column;
			double InY = WallSettings::WallOffset * Row;
			if (!TrapInstance->GetName().Contains("Cones") && !TrapInstance->GetName().Contains("Laser"))
			{
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
			}
			SpawnPoint = FVector{InX, InY, Interactables::TrapsSpawnHeight};
		}
		else if (ObjectClass == AWeapon::StaticClass() || ObjectClass == ABasePickup::StaticClass())
		{
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
						if (ObjectClass == ABasePickup::StaticClass()) SpawnRotation = {0, 0, 0};
						else SpawnRotation = {0, 0, 0};
						break;
					}
					else if (Labyrinth->LabyrinthStructure[Row + j][Column] == 1)
					{
						InX = WallSettings::WallOffset * (Column);
						InY = WallSettings::WallOffset * (Row + j) - (j * Weapons::DistanceFromWall);
						if (ObjectClass == ABasePickup::StaticClass()) SpawnRotation = {0, 90, 0};
						else SpawnRotation = {0, 90, 0};
						break;
					}
				}
			}
			SpawnPoint = FVector{InX, InY, Weapons::SpawnHeight};
		}
		else
		{
			SpawnPoint = FVector{
				WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
				EnemySettings::SpawnHeight
			};
		}
		UWorld* World = GetWorld();
		if (World == nullptr) return "World is null";
		FActorSpawnParameters SpawnParams = FActorSpawnParameters();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		try
		{
			if (SpawnableActors[Index] == nullptr)
			{
				UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("SpawnableActors[Index] is null, check the array"));
				return "";
			}
			if (Index >= SpawnableActors.Num() || Index < 0)
			{
				UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Index out of bounds, check the array size"));
				return "";
			}
			AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(SpawnableActors[Index], SpawnPoint, SpawnRotation, SpawnParams);
			if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
			else
			{
				if (ActorSpawned->Implements<UFreezableActor>())
				{
					FreezableActors.Add(ActorSpawned);
				}
				SpawnedActors.Add(ActorSpawned);
				MovableActors.Add(ActorSpawned);

				if (ObjectClass == ARangedEnemy::StaticClass() || ObjectClass == AMeleeEnemy::StaticClass())
				{
					ABaseEnemy* EnemyInstance = Cast<ABaseEnemy>(ActorSpawned);
					if (EnemyInstance == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("EnemyInstance is null"))
					EnemyInstance->SetLabyrinthMatrix(Labyrinth);
					EnemyInstance->SetMatrixPosition(Row, Column);
				}

				UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Actor spawned: %s"), *ActorSpawned->GetName());
			}
		}
		catch (const std::exception& e)
		{
			UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Crashing Error Actor not spawned"));
			UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Exception: %hs"), e.what());
		}
	}
	return "";
}

FString ASpawnManager::SpawnWeapons()
{
	if (WeaponsClasses.IsEmpty()) UE_LOG(LabyrAInthVR_Scene_Log, Error,
	                                     TEXT("Non blocking scene Error: No Weapons Set"));
	if (PickupsClasses.IsEmpty()) UE_LOG(LabyrAInthVR_Scene_Log, Error,
	                                     TEXT("Non blocking scene Error: No Pickups Set"));
	if (Labyrinth == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Blocking scene Error: Labyrinth is null"));
	if (PlayerStartIndexPosition == -1)
	{
		UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Did not found the player start position, invalid matrix"));
		return "";
	}

	int Row = -1;
	int Column = -1;
	UUtils::ConvertToRowColumn(PlayerStartIndexPosition, Row, Column);
	WeaponsLocations.Insert(PlayerStartIndexPosition, 0);
	for (const int Location : WeaponsLocations)
	{
		int LocationRow = -1;
		int LocationColumn = -1;
		UUtils::ConvertToRowColumn(Location, LocationRow, LocationColumn);
		UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("WeaponsLocations: Row: %d  Column: %d"), LocationRow, LocationColumn);
	}
	NumOfWeaponsSpawned++;

	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->IsInVR())
	{
		FString ErrorString = SpawnActors(WeaponsLocations, WeaponsClasses);
		if (ErrorString != "") return ErrorString;
	}
	else
	{
		FString ErrorString = SpawnActors(WeaponsLocations, PickupsClasses);
		if (ErrorString != "") return ErrorString;
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
	const FVector SpawnPoint = FVector{
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
	EndPortalPosition = SpawnPoint;
	EndPortalRotation = {0, YawRotation, 0};
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(Portal, SpawnPoint, FRotator(0, YawRotation, 0), SpawnParams);
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	SpawnedActors.Add(ActorSpawned);
	MovableActors.Add(ActorSpawned);
	return "";
}

FString ASpawnManager::SpawnPlayerStart()
{
	if (PlayerStartIndexPosition == -1) return "Did not found the player start position, invalid matrix";
	if (PlayerSpawnPoint == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error,
	                                        TEXT("Non blocking scene Error: PlayerSpawnPoint BP not set"));
	int Row = -1;
	int Column = -1;
	UUtils::ConvertToRowColumn(PlayerStartIndexPosition, Row, Column);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Player Start Position = Row: %d  Column: %d"), Row, Column);
	const FVector SpawnPoint = FVector{
		WallSettings::WallOffset * Column, WallSettings::WallOffset * Row,
		0.0
	};
	PlayerStartPosition = {SpawnPoint.X, SpawnPoint.Y, SpawnPoint.Z + 110};
	PlayerStartRotation = {0, 0, 0};
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* ActorSpawned = GetWorld()->SpawnActor<AActor>(PlayerSpawnPoint, SpawnPoint, FRotator(0, 0, 0), SpawnParams);
	if (ActorSpawned == nullptr) UE_LOG(LabyrAInthVR_Scene_Log, Error, TEXT("Actor not spawned, check collisions"))
	MovableActors.Add(ActorSpawned);
	SpawnedActors.Add(ActorSpawned);
	return "";
}

void ASpawnManager::RemoveFromList(AActor* ActorToRemove)
{
	if (!ActorToRemove->IsA<AProceduralSplineWall>())
		MovableActors.Remove(ActorToRemove);

	SpawnedActors.Remove(ActorToRemove);
}

void ASpawnManager::TriggerFrozenStar()
{
	MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(this, AMainCharacter::StaticClass()));

	float PauseDuration = 15.f;
	
	if (IsValid(MainCharacter))
	{
		MainCharacter->SetIsFrozen(true);
		if(IsValid(MainCharacter->GetPlayerStatistics()))
		{
			MainCharacter->GetPlayerStatistics()->PauseLevelTimerForDuration(PauseDuration);
		}
	}
	
	if (FreezableActors.Num() <= 0) return;

	for (const auto& FreezableActor : FreezableActors)
	{
		if (FreezableActor == nullptr) continue;
		if (!FreezableActor->Implements<UFreezableActor>()) continue;
		Cast<IFreezableActor>(FreezableActor)->Freeze(PauseDuration);
	}
}

void ASpawnManager::TriggerCompass(UParticleSystem* CompassEffect)
{
	if (IsValid(CompassInstance))
	{
		CompassInstance->Deactivate();
	}
	
	MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(this, AMainCharacter::StaticClass()));

	if (!IsValid(MainCharacter)) return;

	FVector ToPortal = (EndPortalPosition - MainCharacter->GetActorLocation()).GetSafeNormal() * 500.f;

	//DrawDebugLine(GetWorld(), MainCharacter->GetActorLocation(), ToPortal, FColor::Red, true);

	if (!IsValid(CompassEffect)) return;

	CompassInstance = UGameplayStatics::SpawnEmitterAtLocation(this, CompassEffect, MainCharacter->GetActorLocation(),
															   ToPortal.Rotation());
}

void ASpawnManager::GetNumOfActorSpawned(int& NumOfEnemies, int& NumOfTraps, int& NumOfPowerUps,
                                         int& NumOfWeapons) const
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
