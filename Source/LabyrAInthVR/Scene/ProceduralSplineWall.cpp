#include "ProceduralSplineWall.h"

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

void AProceduralSplineWall::UpdateSplineMesh_Implementation()
{
	
}

void AProceduralSplineWall::EditWallLook_Implementation(uint8 WallType)
{
	
}

void AProceduralSplineWall::AddPillarAtEnd_Implementation(bool bAdd)
{
	
}

void AProceduralSplineWall::AddWallTop_Implementation(bool bAdd)
{
	
}

void AProceduralSplineWall::AddSplinePoint(FVector& Location)
{
	if(SplineComponent == nullptr) return;

	SplineComponent->AddSplinePoint(Location, ESplineCoordinateSpace::World, true);
}

void AProceduralSplineWall::AddSplinePointAtIndex(FVector& Location, int32 Index)
{
	if(SplineComponent == nullptr) return;

	SplineComponent->AddSplinePointAtIndex(Location, Index, ESplineCoordinateSpace::World, true);
}

void AProceduralSplineWall::ClearSplinePoints()
{
	if(SplineComponent == nullptr) return;
	SplineComponent->ClearSplinePoints(true);
}

bool AProceduralSplineWall::GetBackwardNeighbor(FVector& Location, ETravellingDirection TravellingDirection)
{
	if(SplineComponent == nullptr) return false;
	int32 TotalPoints = SplineComponent->GetNumberOfSplinePoints();
	FVector BackwardOffset;

	switch(TravellingDirection)
	{
	case ETravellingDirection::ETD_Horizontal:
		BackwardOffset = FVector(-500.f, 0.f, 0.f);
		break;
	case ETravellingDirection::ETD_Vertical:
		BackwardOffset = FVector(0.f, -500.f, 0.f);
		break;
	}
	
	for(int i = 0; i < TotalPoints; i++)
	{
		FTransform SplinePointTrasform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector OffsetLocation = Location + BackwardOffset;
		if(SplinePointTrasform.GetLocation() == OffsetLocation) return true;
	}
	return false;
}

void AProceduralSplineWall::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetTimerManager().SetTimer(CreatePointsTimerHandle, this, &ThisClass::CreatePointsTimerFinished, 2.f);
}

void AProceduralSplineWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProceduralSplineWall::AddPillarAtStart_Implementation(bool bAdd)
{
	
}

void AProceduralSplineWall::CreatePointsTimerFinished()
{
	SplineComponent->ClearSplinePoints();
	
	SplineComponent->AddSplinePoint(FVector(500.f, 500.f, 0.f), ESplineCoordinateSpace::Local, true);
	
	SplineComponent->AddSplinePoint(FVector(500.f, 1000.f, 0.f), ESplineCoordinateSpace::Local, true);

	SplineComponent->AddSplinePoint(FVector(500.f, 1000.f, 0.f), ESplineCoordinateSpace::Local, true);
	
	UpdateSplineMesh();
}

