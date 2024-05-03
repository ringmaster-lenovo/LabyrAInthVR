// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "SpawnManager.generated.h"


class IMovableActor;
class UFreezableActor;
class IFreezableActor;
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
class LABYRAINTHVR_API ASpawnManager : public AActor, public ISpawnableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	int PowerUpsToSpawn = 0;
	int TrapsToSpawn = 0;
	int EnemiesToSpawn = 0;
	int WeaponsToSpawn = 0;
	
	int NumOfPowerUpsSpawned = 0;
	int NumOfTrapsSpawned = 0;
	int NumOfEnemiesSpawned = 0;
	int NumOfWeaponsSpawned = 0;
	
	TArray<int> PotentialPowerUpSpawnLocations = {};
	TArray<int> PotentialTrapSpawnLocations = {};
	TArray<int> PotentialEnemySpawnLocations = {};
	TArray<int> PotentialWeaponSpawnLocations = {};
	
	TArray<int> PowerUpsLocations = {};
	TArray<int> TrapsLocations = {};
	TArray<int> EnemiesLocations = {};
	TArray<int> WeaponsLocations = {};
	
	int PlayerStartIndexPosition = -1;
	int PortalIndexPosition = -1;

	UPROPERTY()
	ULabyrinthDTO* Labyrinth;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "PowerUps")
	TArray<TSubclassOf<AActor>> PowerUpsClasses;

	UPROPERTY(EditAnywhere, Category = "Traps")
	TArray<TSubclassOf<AActor>> TrapsClasses;

	UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray<TSubclassOf<AActor>> EnemiesClasses;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TArray<TSubclassOf<AActor>> WeaponsClasses;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TArray<TSubclassOf<AActor>> PickupsClasses;

	UPROPERTY(EditAnywhere, Category = "Portals")
	TSubclassOf<AActor> PlayerSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Portals")
	TSubclassOf<AActor> Portal;

	UPROPERTY()
	TArray<AActor*> FreezableActors;

	UPROPERTY()
	TArray<AActor*> MovableActors;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;
	
	UFUNCTION(BlueprintCallable)
	void TriggerFrozenStar();

	UFUNCTION(BlueprintCallable)
	void TriggerCompass(UParticleSystem* CompassEffect);

	UFUNCTION(BlueprintCallable)
	void RemoveFromList(AActor* ActorToRemove);
	
	FVector PlayerStartPosition;

	FRotator PlayerStartRotation;
	
	void UpdateSpawnableActor(AActor* SpawnedWall);
	
	FString SpawnActorsInLabyrinth(const ULabyrinthDTO* LabyrinthDtoReference);

	void FindPotentialSpawnLocations(const ULabyrinthDTO* LabyrinthDTO, int Row, int Column);

	FString ChoosePowerUpsSpawnPoints(const int NumOfPowerUpsToSpawn);

	FString ChooseTrapsSpawnPoints(const int NumOfTrapsToSpawn);

	FString ChooseEnemiesSpawnPoints(const int NumOfEnemiesToSpawn);
	
	FString ChooseRandomSpawnLocation(int NumOfActorsToSpawn, TArray<int>& ActorsSpawnLocations,  TArray<int>& PotentialLocations, uint8 ConventionalValueInTheMatrix) const;

	FString DifficultyDecider();

	FString SpawnActors(const TArray<int>& SpawnLocations, const TArray<TSubclassOf<AActor>>& SpawnableActors);

	FString SpawnWeapons();

	FString SpawnPortal();

	FString SpawnPlayerStart();

	void GetNumOfActorSpawned(int &NumOfEnemies, int &NumOfTraps, int &NumOfPowerUps, int &NumOfWeapons) const;
};
