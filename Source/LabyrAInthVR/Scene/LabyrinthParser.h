#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"
#include "LabyrinthParser.generated.h"

class ABaseEnemy;
class ASceneController;
class AProceduralSplineWall;

UENUM(BlueprintType)
enum class ETravellingDirection : uint8
{
	ETD_Vertical UMETA(DisplayName="Travelling Vertical"),
	ETD_Horizontal UMETA(DisplayName="Travelling Horizontal"),
	ETD_Flat UMETA(DisplayName="Flat")
};

UENUM()
enum EWallPalette
{
	EWP_Aqua UMETA(DisplayName="Blue Palette"),
	// Add custom palette here
	EWP_Autumn UMETA(DisplayName="Red Palette"),
};

UCLASS()
class LABYRAINTHVR_API ALabyrinthParser : public AActor
{
	GENERATED_BODY()

public:
	ALabyrinthParser();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	bool BuildLabyrinth(const std::vector<std::vector<uint8>>& Matrix);
	FVector GetNextDestination(uint8& Row, uint8& Column, EEnemyDirection& LastDirection);

private:
	void TravelHorizontal(uint8 RowIndex, uint8 FinalColumnIndex, ETravellingDirection TravellingDirection,
	                      float XOffset, float YOffset);
	void TravelVertical(uint8 ColumnIndex, uint8 FinalRowIndex, ETravellingDirection TravellingDirection, float XOffset,
	                    float YOffset);
	void SpawnPillarAtIntersection(uint8 RowIndex, uint8 ColumnIndex, FVector& SpawnLocation,
	                               ETravellingDirection TravellingDirection);
	void SpawnFlatSurface(bool bFloor);
	void BuildLabyrinthInternal();
	void SetShortWallSettings(AProceduralSplineWall* & ProceduralSplineWall);
	bool HasFrontNeighbor(uint8 Row, uint8 Column, ETravellingDirection TravellingDirection) const;
	bool IsIntersection(uint8 Row, uint8 Column) const;
	bool IsDiagonal(uint8 Row, uint8 Column) const;
	bool IsInRoom(uint8 Row, uint8 Column, const TArray<EEnemyDirection>& FreeEnemyDirections);
	bool CheckForExit(uint8 Row, uint8 Column, EEnemyDirection EnemyDirection);
	
	void FillExitIndexes(uint8& Row, uint8& Column, EEnemyDirection EnemyDirection);
	void FillDiagonalIndexes(uint8& Row, uint8& Column);
	void FillDiagonalMatrix(uint8 Row, uint8 Column, TArray<EEnemyDiagonalDirection>& EnemyDiagonalDirections);
	AProceduralSplineWall* SpawnWall(FVector& Location, ETravellingDirection TravellingDirection, uint8 WallType);
	AProceduralSplineWall* GetPossibleNeighborhood(FVector& Location, ETravellingDirection TravellingDirection) const;

	void FillFreeDirections(uint8 Row, uint8 Column, TArray<EEnemyDirection>& FreeEnemyDirections);
	void ChooseNextDirection(TArray<EEnemyDirection>& EnemyDirections, EEnemyDirection& NextDirection, EEnemyDirection PreviousDirection, uint8 MinIndex, uint8 MaxIndex);
	EEnemyDirection GetOppositeDirection(EEnemyDirection EnemyDirection);
	EEnemyDirection GetIntersecDirection(TArray<EEnemyDirection>& EnemyDirections, EEnemyDirection EnemyDirection);
	
	static FVector GetRandomHex(EWallPalette Palette);

	UPROPERTY(EditAnywhere, Category=ProceduralWall)
	TSubclassOf<AActor> ProceduralWallClass;

	std::vector<std::vector<uint8>> UnparsedLabyrinthMatrix;

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

	EWallPalette ChosenPalette{EWP_Aqua};

	float Timer{0.f};

	// Enemy
	UPROPERTY(EditAnywhere, Category=Enemy)
	TSubclassOf<ABaseEnemy> BaseEnemyClass;

	UPROPERTY()
	TArray<ABaseEnemy*> SpawnedEnemies {};
};
