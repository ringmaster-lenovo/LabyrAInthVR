#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneController.h"
#include "LabyrinthParser.generated.h"

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
	AProceduralSplineWall* SpawnWall(FVector& Location, ETravellingDirection TravellingDirection, uint8 WallType);
	AProceduralSplineWall* GetPossibleNeighborhood(FVector& Location, ETravellingDirection TravellingDirection) const;
	
	static FVector GetRandomHex(EWallPalette Palette);

	UPROPERTY(EditAnywhere)
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
	UPROPERTY(EditAnywhere)
	TArray<uint8> OuterWallMaterials;

	uint8 ChosenOuterWallMaterial{0};

	UPROPERTY(EditAnywhere)
	TArray<uint8> InsideWallMaterials;

	uint8 ChosenInsideWallMaterial{0};

	UPROPERTY(EditAnywhere)
	TArray<uint8> FlatWallMaterials;

	uint8 ChosenFlatWallMaterial{0};

	UPROPERTY(EditAnywhere)
	TArray<uint8> PillarMaterials;

	uint8 ChosenPillarMaterial{0};

	UPROPERTY(EditAnywhere)
	TArray<uint8> ShortWallMaterials;

	EWallPalette ChosenPalette{EWP_Aqua};
};
