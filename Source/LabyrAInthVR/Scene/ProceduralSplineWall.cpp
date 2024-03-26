#include "ProceduralSplineWall.h"

#include "Config.h"
#include "LabyrinthParser.h"
#include "Components/SplineComponent.h"

AProceduralSplineWall::AProceduralSplineWall()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(DefaultRoot);
}

void AProceduralSplineWall::BeginPlay()
{
	Super::BeginPlay();
}

void AProceduralSplineWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProceduralSplineWall::UpdateSplineMesh_Implementation()
{
}

void AProceduralSplineWall::EditWallLook_Implementation(uint8 WallType, FVector RGB, float WallAge)
{
}

void AProceduralSplineWall::AddPillarAtEnd_Implementation(bool bAdd)
{
}

void AProceduralSplineWall::AddWallTop_Implementation(bool bAdd)
{
}

void AProceduralSplineWall::EditPillarLook_Implementation(uint8 PillarType, FVector RGB, float PillarAge)
{
}

void AProceduralSplineWall::AddSplinePoint(FVector& Location)
{
	if (SplineComponent == nullptr) return;

	SplineComponent->AddSplinePoint(Location, ESplineCoordinateSpace::World, true);
}

void AProceduralSplineWall::AddSplinePointAtIndex(FVector& Location, int32 Index)
{
	if (SplineComponent == nullptr) return;

	SplineComponent->AddSplinePointAtIndex(Location, Index, ESplineCoordinateSpace::World, true);
}

void AProceduralSplineWall::ClearSplinePoints()
{
	if (SplineComponent == nullptr) return;
	SplineComponent->ClearSplinePoints(true);
}

bool AProceduralSplineWall::GetBackwardNeighbor(FVector& Location, ETravellingDirection TravellingDirection)
{
	if (SplineComponent == nullptr) return false;
	int32 TotalPoints = SplineComponent->GetNumberOfSplinePoints();
	FVector BackwardOffset;

	switch (TravellingDirection)
	{
	case ETravellingDirection::Etd_Horizontal:
		BackwardOffset = FVector(-WallSettings::WallOffset, 0.f, 0.f);
		break;
	case ETravellingDirection::Etd_Vertical:
		BackwardOffset = FVector(0.f, -WallSettings::WallOffset, 0.f);
		break;
	default:
		break;
	}

	for (int i = 0; i < TotalPoints; i++)
	{
		FTransform SplinePointTrasform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector OffsetLocation = Location + BackwardOffset;
		if (SplinePointTrasform.GetLocation() == OffsetLocation) return true;
	}
	return false;
}

void AProceduralSplineWall::AddPillarAtStart_Implementation(bool bAdd)
{
}
