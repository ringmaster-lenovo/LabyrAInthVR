#pragma once

#include "CoreMinimal.h"
#include "LabyrinthParser.h"
#include "SpawnManager.h"
#include "GameFramework/Actor.h"
#include "SceneController.generated.h"

class ANavMeshBoundsVolume;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Scene_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API ASceneController : public AActor
{
	GENERATED_BODY()

public:
	ASceneController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	FString SetupLevel(ULabyrinthDTO* LabyrinthDTO);
	
	FString SetupDemoLevel();

	FString CleanUpOnlyLevel() const;
	
	FString CleanUpLevelAndDoStatistics(int &NumOfEnemiesKilled, int &NumOfTrapsExploded, int &NumOfPowerUpsCollected, int &NumOfWeaponsFound) const;

	FString RespawnMovableActors(ULabyrinthDTO* LabyrinthDto) const;

	DECLARE_MULTICAST_DELEGATE(FSceneReadyEvent);
	FSceneReadyEvent OnSceneReady;

	DECLARE_MULTICAST_DELEGATE(FSceneCleanedUpEvent);
	FSceneCleanedUpEvent OnSceneCleanedUp;

	DECLARE_MULTICAST_DELEGATE(FRespawnActorsEvent);
	FRespawnActorsEvent OnActorsRespawned;

	UFUNCTION()
	void GetPlayerStartPositionAndRotation(FVector& PlayerStartPosition, FRotator& PlayerStartRotation) const;

	void GetPlayerDemoStartPositionAndRotation(FVector& PlayerStartPosition, FRotator& PlayerStartRotation) const;

	void FreezeAllActors(bool bFreeze);

	//PowerUp Utils
	void RemoveFromSpawnManagerList(AActor* ActorToRemove);
	
	UFUNCTION()
	void GetEndPortalPositionAndRotation(FVector& PlayerStartPosition, FRotator& PlayerStartRotation) const;

private:
	UPROPERTY()
	ALabyrinthParser* LabyrinthParser;

	UPROPERTY()
	ASpawnManager* SpawnManager;
	
	UPROPERTY()
	ANavMeshBoundsVolume* NavMeshBoundsVolume;
	
	UFUNCTION()
	void UpdateNavMeshBoundsPosition();

	UPROPERTY(EditAnywhere)
	float NavMeshDelayTimer{0.25f};
	
	FTimerHandle NavMeshBoundsTimerHandle;

	void UpdateNavMeshBoundsVolume(const ULabyrinthDTO* LabyrinthDto);
	
	UPROPERTY(EditAnywhere, Category = "Scene")
	TSubclassOf<ALabyrinthParser> LabyrinthParser_BP;

	UPROPERTY(EditAnywhere, Category = "Scene")
	TSubclassOf<ASpawnManager> SpawnManager_BP;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	UPROPERTY()
	TArray<AActor*> MovableActors;

	UPROPERTY()
	TArray<AActor*> FreezableActors;

public:
	FORCEINLINE void SetSpawnedActors(TArray<AActor*> Spawned) {SpawnedActors = Spawned; }
	FORCEINLINE void SetMovableActors(TArray<AActor*> Spawned) {MovableActors = Spawned; }
	FORCEINLINE void SetFreezableActors(TArray<AActor*> Spawned) {FreezableActors = Spawned; }
};
