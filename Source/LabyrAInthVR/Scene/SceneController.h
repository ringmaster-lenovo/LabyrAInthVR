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

	DECLARE_MULTICAST_DELEGATE(FSceneReadyEvent);
	FSceneReadyEvent OnSceneReady;

private:
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
	
};
