// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Network/LabyrinthDTO.h"
#include "SpawnManager.generated.h"


class APowerUp;
class ATrap;
class ABaseEnemy;

UENUM()
enum EActorToSpawn { PowerUps, Traps, Enemies };

UENUM(BlueprintType)
enum class EPowerUpsTypes : uint8
{
	Ept_Heart        UMETA(DisplayName="Heart"),
	Ept_Shield       UMETA(DisplayName="Shield"),
	Ept_Lightning    UMETA(DisplayName="Lightning"),
	Ept_FrozenStar   UMETA(DisplayName="Frozen Star"),
	Ept_Diamond	     UMETA(DisplayName="Diamond Compass")
};

UENUM(BlueprintType)
enum class ETrapsTypes : uint8
{
	Ett_TripWire     UMETA(DisplayName="TripWire"),
	Ett_Blades       UMETA(DisplayName="Blades"),
	Ett_Cannon       UMETA(DisplayName="Cannon"),
	Ett_MachineGun   UMETA(DisplayName="MachineGun"),
	Ett_Laser	     UMETA(DisplayName="Laser"),
	Ett_Spike	     UMETA(DisplayName="Spike"),
	Ett_Bomb	     UMETA(DisplayName="Bomb")
};

UENUM(BlueprintType)
enum class EEnemyTypes : uint8
{
	Eet_MinionBlue   UMETA(DisplayName="MinionBlue"),
	Eet_MinionRed    UMETA(DisplayName="MinionRed"),
	Eet_MinionWhite  UMETA(DisplayName="MinionWhite"),
	Eet_MinionGreen  UMETA(DisplayName="MinionGreen"),
	Eet_Boss	     UMETA(DisplayName="Boss")
};

UCLASS()
class LABYRAINTHVR_API ASpawnManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	uint8 DifficultyLevel = 0;
	
	int NumOfPowerUpsSpawned = 0;
	int NumOfTrapsSpawned = 0;
	int NumOfEnemiesSpawned = 0;
	
	TArray<int> PowerUpsLocations = {};
	TArray<int> TrapsLocations = {};
	TArray<int> EnemiesLocations = {};

	int PlayerStartPosition = -1;
	int ExitPosition = -1;
	
	TArray<int> PotentialPowerUpSpawnLocations = {};
	TArray<int> PotentialTrapSpawnLocations = {};
	TArray<int> PotentialEnemySpawnLocations = {};
	
	TMap<TSubclassOf<APowerUp>, int> PowerUpsComplexityMap = {};
	TMap<TSubclassOf<ATrap>, int> TrapsComplexityMap = {};
	TMap<TSubclassOf<ABaseEnemy>, int> EnemiesComplexityMap = {};

	UPROPERTY()
	ULabyrinthDTO* LabyrinthDTO;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<TSubclassOf<AActor>> PowerUpsClasses;

	UPROPERTY(EditAnywhere, Category = "Traps")
	TArray<TSubclassOf<AActor>> TrapsClasses;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray<TSubclassOf<AActor>> EnemiesClasses;

	void SetDifficultyLevel(const uint8 Level) { DifficultyLevel = Level; }

	FString SpawnActorsInLabyrinth(ULabyrinthDTO* LabyrinthDTOReference);

	void FindPotentialSpawnLocations(const ULabyrinthDTO* LabyrinthDTO, int Row, int Column);

	FString ChoosePowerUpsSpawnPoints(const int NumOfPowerUpsToSpawn);

	FString ChooseTrapsSpawnPoints(const int NumOfTrapsToSpawn);

	FString ChooseEnemiesSpawnPoints(const int NumOfEnemiesToSpawn);
	
	FString ChooseRandomSpawnLocation(int NumOfActorsToSpawn, TArray<int>& ActorsSpawnLocations,  TArray<int>& PotentialLocations, uint8 ConventionalValueInTheMatrix);

	FString ComplexityDecider(int& PowerUpsToSpawn, int& TrapsToSpawn, int& EnemiesToSpawn);

	FString SpawnActors(const TArray<int>& SpawnLocations, const TMap<TSubclassOf<AActor>, int>& ActorsToSpawn);
};
