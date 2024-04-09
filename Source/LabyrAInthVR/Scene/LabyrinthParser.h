#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"
#include "LabyrinthParser.generated.h"

class ABaseEnemy;
class ASceneController;
class ASpawnManager;
class ULabyrinthDTO;
class AProceduralSplineWall;

UENUM(BlueprintType)
enum class ETravellingDirection : uint8
{
	Etd_Vertical UMETA(DisplayName="Travelling Vertical"),
	Etd_Horizontal UMETA(DisplayName="Travelling Horizontal"),
	Etd_Flat UMETA(DisplayName="Flat")
};

UENUM()
enum EWallPalette
{
	Ewp_Aqua UMETA(DisplayName="Blue Palette"),
	// Add custom palette here
	Ewp_Autumn UMETA(DisplayName="Red Palette"),
};

UCLASS()
class LABYRAINTHVR_API ALabyrinthParser : public AActor
{
	GENERATED_BODY()

public:
	ALabyrinthParser();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	FString BuildLabyrinth(ULabyrinthDTO* LabyrinthDTOReference, ASpawnManager* SpawnManagerReference);

private:
	void BuildLabyrinthInternal();
	void TravelHorizontal(uint8 RowIndex, uint8 FinalColumnIndex, ETravellingDirection TravellingDirection,
	                      float XOffset, float YOffset);
	void TravelVertical(uint8 ColumnIndex, uint8 FinalRowIndex, ETravellingDirection TravellingDirection, float XOffset,
	                    float YOffset);
	void SpawnPillarAtIntersection(uint8 RowIndex, uint8 ColumnIndex, FVector& SpawnLocation,
	                               ETravellingDirection TravellingDirection);
	void SpawnFlatSurface(bool bFloor);
	void SetShortWallSettings(AProceduralSplineWall* & ProceduralSplineWall);
	bool HasFrontNeighbor(uint8 Row, uint8 Column, ETravellingDirection TravellingDirection) const;
	
	AProceduralSplineWall* SpawnWall(FVector& Location, ETravellingDirection TravellingDirection, uint8 WallType);
	AProceduralSplineWall* GetPossibleNeighborhood(FVector& Location, ETravellingDirection TravellingDirection) const;
	
	static FVector GetRandomHex(EWallPalette Palette);

	UPROPERTY(EditAnywhere, Category=ProceduralWall)
	TSubclassOf<AActor> ProceduralWallClass;

	UPROPERTY()
	ULabyrinthDTO* LabyrinthDTO;

	UPROPERTY()
	ASpawnManager* SpawnManager;
	
	// Runtime instances
	UPROPERTY()
	TArray<AProceduralSplineWall*> ProceduralSplineWallInstancesVertical;

	UPROPERTY()
	TArray<AProceduralSplineWall*> ProceduralSplineWallInstancesHorizontal;

	UPROPERTY()
	TArray<AProceduralSplineWall*> ProceduralSplineWallInstancesFlats;

	// Materials
	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> OuterWallMaterials;

	uint8 ChosenOuterWallMaterial{0};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> InsideWallMaterials;

	uint8 ChosenInsideWallMaterial{0};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> FlatWallMaterials;

	uint8 ChosenFlatWallMaterial{0};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> PillarMaterials;

	uint8 ChosenPillarMaterial{0};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> ShortWallMaterials;

	EWallPalette ChosenPalette{Ewp_Aqua};

	float Timer{0.f};

	// Enemy
	UPROPERTY(EditAnywhere, Category=Enemy)
	TArray<TSubclassOf<ABaseEnemy>> BaseEnemyClass;

	UPROPERTY()
	TArray<ABaseEnemy*> SpawnedEnemies {};

	UPROPERTY(EditAnywhere)
	uint8 Enemies{0};
};
