#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrinthParser.generated.h"

class AProceduralSplineWall;

UENUM(BlueprintType)
enum class EWallType : uint8
{
	EWT_None = 0 UMETA(DisplayName="None"),
	EWT_HalfEast = 3 UMETA(DisplayName="Half East"),
	EWT_HalfNorth = 5 UMETA(DisplayName="Half North"),
	EWT_HalfOvest = 7 UMETA(DisplayName="Half Ovest"),
	EWT_HalfSouth = 11 UMETA(DisplayName="Half South"),

	EWT_FullHorizontal = 10 UMETA(DisplayName = "Full Horizontal"),
	EWT_FullVertical = 16 UMETA(DisplayName = "Full Vertical"),
	
	EWT_BottomLeftCorner = 8 UMETA(DisplayName="Bottom Left Corner"),
	EWT_UpperLeftCorner = 14 UMETA(DisplayName="Upper Left Corner"),
	EWT_UpperRightCorner = 18 UMETA(DisplayName="Upper Right Corner"),
	EWT_BottomRightCorner = 12 UMETA(DisplayName="Bottom Right Corner"),

	EWT_BottomT = 15 UMETA(DisplayName="Bottom T"),
	EWT_LeftT = 19 UMETA(DisplayName="Left T"),
	EWT_UpperT = 21 UMETA(DisplayName="Upper T"),
	EWT_RightT = 23 UMETA(DisplayName="Right T"),

	EWT_FullCross = 26 UMETA(DisplayName="Full Cross"),
};

UENUM(BlueprintType)
enum class ETravellingDirection : uint8
{
	ETD_Vertical UMETA(DisplayName="Travelling Vertical"),
	ETD_Horizontal UMETA(DisplayName="Travelling Horizontal"),
	ETD_Flat UMETA(DisplayName="Flat")
};


USTRUCT()
struct FWallSpawnResult{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	AProceduralSplineWall* WallInstance;

	bool bHasLeftNeighbor = false;
};

UCLASS()
class LABYRAINTH_API ALabyrinthParser : public AActor
{
	GENERATED_BODY()
	
public:	
	ALabyrinthParser();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	void BuildLabyrinth(uint8 *MatrixPointer, uint8 Rows, uint8 Columns);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProceduralWallClass;

	UPROPERTY()
	TArray<AProceduralSplineWall*> ProceduralSplineWallInstancesVertical;

	UPROPERTY()
	TArray<AProceduralSplineWall*> ProceduralSplineWallInstancesHorizontal;

	UPROPERTY()
	TArray<AProceduralSplineWall*> ProceduralSplineWallInstancesFlats;

	UPROPERTY()
	TArray<AProceduralSplineWall*> MergedWalls;

	UPROPERTY(EditAnywhere)
	TArray<uint8> OuterWallMaterials;

	uint8 ChosenOuterWallMaterial = 0;

	UPROPERTY(EditAnywhere)
	TArray<uint8> InsideWallMaterials;

	uint8 ChosenInsideWallMaterial = 0;

	std::vector<std::vector<uint8>> UnparsedLabyrinthMatrix;

private:
	void TravelHorizontal(uint8 RowIndex, uint8 FinalColumnIndex, ETravellingDirection TravellingDirection, float XOffset, float YOffset);
	void TravelVertical(uint8 ColumnIndex, uint8 FinalRowIndex, ETravellingDirection TravellingDirection, float XOffset, float YOffset);
	AProceduralSplineWall* SpawnWall(FVector& Location, ETravellingDirection TravellingDirection, uint8 WallType);
	AProceduralSplineWall* GetPossibleNeighborhood(FVector& Location, ETravellingDirection TravellingDirection);
	bool HasFrontNeighbor(uint8 Row, uint8 Column, ETravellingDirection TravellingDirection);
	void SpawnCrossIntersection(uint8 RowIndex, uint8 ColumnIndex, FVector& SpawnLocation, ETravellingDirection TravellingDirection);
	void SpawnFlatSurface(bool bFloor);
	void BuildLabyrinthInternal();
	// TO DELETE
	void MergeWalls(TArray<AProceduralSplineWall*>& FirstWalls, TArray<AProceduralSplineWall*>& SecondWalls, bool bSelfMerge);
};
