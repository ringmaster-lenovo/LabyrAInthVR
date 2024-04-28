#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "LabyrinthParser.generated.h"

class ABaseEnemy;
class ASceneController;
class ASpawnManager;
class ULabyrinthDTO;
class AProceduralSplineWall;

UENUM(BlueprintType)
enum class ETravellingDirection : uint8
{
	Etd_Vertical    UMETA(DisplayName="Travelling Vertical"),
	Etd_Horizontal  UMETA(DisplayName="Travelling Horizontal"),
	Etd_Flat        UMETA(DisplayName="Flat")
};

UENUM()
enum EWallPalette
{
	Ewp_Aqua        UMETA(DisplayName="Blue Palette"),
	Ewp_Autumn      UMETA(DisplayName="Red Palette"),
	Ewp_Rose        UMETA(DisplayName="Rose Palette"),
	Ewp_BlueYellow  UMETA(DisplayName="Blue Yellow Palette"),
	Ewp_Reddish     UMETA(DisplayName="Reddish Palette"),
};

UCLASS()
class LABYRAINTHVR_API ALabyrinthParser : public AActor, public ISpawnableActor
{
	GENERATED_BODY()

public:
	ALabyrinthParser();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	FString BuildLabyrinth(ULabyrinthDTO* LabyrinthDtoReference, ASpawnManager* SpawnManagerReference);

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
	
	AProceduralSplineWall* SpawnWall(FVector& Location, ETravellingDirection TravellingDirection, uint8 WallType, bool bIsOuterWall = false);
	AProceduralSplineWall* GetPossibleNeighborhood(FVector& Location, ETravellingDirection TravellingDirection) const;
	
	static FVector GetRandomColor(EWallPalette Palette);

	FVector GetRandomColorForOuterWall(EWallPalette Palette);
	
	FVector GetRandomColorForOuterPillar(EWallPalette Palette);

	UPROPERTY(EditAnywhere, Category=ProceduralWall)
	TSubclassOf<AActor> ProceduralWallClass;

	UPROPERTY()
	ULabyrinthDTO* LabyrinthDto;

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

	FVector OuterWallColor {0.f, 0.f, 0.f};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> OuterPillarMaterials;

	uint8 ChosenOuterPillarMaterial{0};

	FVector OuterPillarColor {0.f, 0.f, 0.f};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> InsideWallMaterials;

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> FlatWallMaterials;

	uint8 ChosenFlatWallMaterial{0};

	UPROPERTY(EditAnywhere, Category=Materials)
	TArray<uint8> InsidePillarMaterials;

	uint8 ChosenInsidePillarMaterial{0};

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
