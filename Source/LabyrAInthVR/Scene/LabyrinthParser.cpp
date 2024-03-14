#include "LabyrinthParser.h"

#include "Config.h"
#include "ProceduralSplineWall.h"
#include "SceneController.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

ALabyrinthParser::ALabyrinthParser()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALabyrinthParser::BeginPlay()
{
	Super::BeginPlay();
}

void ALabyrinthParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ALabyrinthParser::BuildLabyrinth(const std::vector<std::vector<uint8>>& Matrix)
{
	if (Matrix.capacity() <= 0) return false;

	const uint8 Rows = std::size(Matrix);
	const uint8 Columns = std::size(Matrix[0]);

	UnparsedLabyrinthMatrix.resize(Rows, std::vector<uint8>(Columns));

	for (uint8 i = 0; i < Rows; i++)
	{
		for (uint8 j = 0; j < Columns; j++)
		{
			UnparsedLabyrinthMatrix[i][j] = Matrix[i][j];
		}
	}

	BuildLabyrinthInternal();
	return true;
}


void ALabyrinthParser::BuildLabyrinthInternal()
{
	// Select material and color palette
	ChosenOuterWallMaterial = OuterWallMaterials[FMath::RandRange(0, OuterWallMaterials.Num() - 1)]; // Perimeter wall
	ChosenFlatWallMaterial = FlatWallMaterials[FMath::RandRange(0, FlatWallMaterials.Num() - 1)]; // Floor & ceiling
	ChosenPillarMaterial = PillarMaterials[FMath::RandRange(0, PillarMaterials.Num() - 1)]; // Pillars
	ChosenPalette = static_cast<EWallPalette>(FMath::RandRange(EWallPalette::EWP_Aqua, EWallPalette::EWP_Autumn));

	// Create meshes
	float Offset = 0.f;

	// Add horizontal splines
	for (uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix); i++)
	{
		TravelHorizontal(i, std::size(UnparsedLabyrinthMatrix[0]), ETravellingDirection::ETD_Horizontal, 0.f,
		                 0.f + Offset);
		Offset += WallSettings::WallOffset;
	}

	Offset = 0.f;

	// Add vertical splines
	for (uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix[0]); i++)
	{
		TravelVertical(i, std::size(UnparsedLabyrinthMatrix), ETravellingDirection::ETD_Vertical, 0.f + Offset, 0.f);
		Offset += WallSettings::WallOffset;
	}

	// Add floor & ceiling
	SpawnFlatSurface(false);
	SpawnFlatSurface(true);

	// Remove overlapping pillars
	for (auto& HorizontalSplineWall : ProceduralSplineWallInstancesHorizontal)
	{
		FVector HorizontalStartingPointLocation = HorizontalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(
			0, ESplineCoordinateSpace::World);
		FVector HorizontalEndingPointLocation = HorizontalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(
			HorizontalSplineWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

		for (auto& VerticalSplineWall : ProceduralSplineWallInstancesVertical)
		{
			FVector VerticalStartingPointLocation = VerticalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(
				0, ESplineCoordinateSpace::World);
			FVector VerticalEndingPointLocation = VerticalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(
				VerticalSplineWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

			if (HorizontalStartingPointLocation == VerticalStartingPointLocation || HorizontalStartingPointLocation ==
				VerticalEndingPointLocation)
			{
				HorizontalSplineWall->AddPillarAtStart(false);
			}

			if (HorizontalEndingPointLocation == VerticalEndingPointLocation || HorizontalEndingPointLocation ==
				VerticalStartingPointLocation)
			{
				HorizontalSplineWall->AddPillarAtEnd(false);
			}
		}
	}

	// Update meshes and look for potential "short" walls
	for (auto& ProceduralSplineWallPair : ProceduralSplineWallInstancesHorizontal)
	{
		if (ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == WallSettings::ShortWall)
			SetShortWallSettings(ProceduralSplineWallPair);

		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	for (auto& ProceduralSplineWallPair : ProceduralSplineWallInstancesVertical)
	{
		if (ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == WallSettings::ShortWall)
			SetShortWallSettings(ProceduralSplineWallPair);

		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	for (const auto& ProceduralSplineWallPair : ProceduralSplineWallInstancesFlats)
		ProceduralSplineWallPair->UpdateSplineMesh();


	ASceneController* SceneController = Cast<ASceneController>(
		UGameplayStatics::GetActorOfClass(this, ASceneController::StaticClass()));

	SceneController->OnSceneReady.Broadcast();
}

void ALabyrinthParser::SetShortWallSettings(AProceduralSplineWall* & ProceduralSplineWall)
{
	const auto WallRandomColor = GetRandomHex(ChosenPalette);
	const auto PillarRandomColor = GetRandomHex(ChosenPalette);
	const float WallAge{FMath::RandRange(0.f, 2.f)};
	const float PillarAge{FMath::RandRange(0.f, 2.f)};
	const auto ShortWallMaterial{ShortWallMaterials[FMath::RandRange(0, ShortWallMaterials.Num() - 1)]};
	ProceduralSplineWall->EditWallLook(
		ShortWallMaterial, WallRandomColor,
		FMath::RandBool() ? WallAge : 0.f);

	ProceduralSplineWall->EditPillarLook(
		ShortWallMaterial, PillarRandomColor,
		FMath::RandBool() ? PillarAge : 0.f);
}

void ALabyrinthParser::TravelHorizontal(uint8 RowIndex, uint8 FinalColumnIndex,
                                        ETravellingDirection TravellingDirection, float XOffset, float YOffset)
{
	for (uint8 j = 0; j < FinalColumnIndex; j++)
	{
		FVector SpawnLocation = FVector(XOffset, YOffset, 0.f);

		AProceduralSplineWall* Neighbor = GetPossibleNeighborhood(SpawnLocation, TravellingDirection);

		if (UnparsedLabyrinthMatrix[RowIndex][j] == 0 || (!HasFrontNeighbor(RowIndex, j, TravellingDirection) &&
			Neighbor == nullptr))
		{
			XOffset += WallSettings::WallOffset;
			continue;
		}

		if (Neighbor == nullptr)
		{
			uint8 WallType = (RowIndex == 0 || RowIndex == std::size(UnparsedLabyrinthMatrix) - 1)
				                 ? ChosenOuterWallMaterial
				                 : InsideWallMaterials[FMath::RandRange(0, InsideWallMaterials.Num() - 1)];

			SpawnWall(SpawnLocation, TravellingDirection, WallType);
		}
		else
			Neighbor->AddSplinePoint(SpawnLocation);

		SpawnPillarAtIntersection(RowIndex, j, SpawnLocation, TravellingDirection);

		XOffset += WallSettings::WallOffset;
	}
}

void ALabyrinthParser::TravelVertical(uint8 ColumnIndex, uint8 FinalRowIndex, ETravellingDirection TravellingDirection,
                                      float XOffset, float YOffset)
{
	for (uint8 i = 0; i < FinalRowIndex; i++)
	{
		FVector SpawnLocation = FVector(XOffset, YOffset, 0.f);

		AProceduralSplineWall* Neighbor = GetPossibleNeighborhood(SpawnLocation, TravellingDirection);

		if (UnparsedLabyrinthMatrix[i][ColumnIndex] == 0 || (!HasFrontNeighbor(i, ColumnIndex, TravellingDirection) &&
			Neighbor == nullptr))
		{
			YOffset += WallSettings::WallOffset;
			continue;
		}

		if (Neighbor == nullptr)
		{
			uint8 WallType = WallType = (ColumnIndex == 0 || ColumnIndex == std::size(UnparsedLabyrinthMatrix[0]) - 1)
				                            ? ChosenOuterWallMaterial
				                            : InsideWallMaterials[FMath::RandRange(0, InsideWallMaterials.Num() - 1)];

			SpawnWall(SpawnLocation, TravellingDirection, WallType);
		}
		else
			Neighbor->AddSplinePoint(SpawnLocation);

		YOffset += WallSettings::WallOffset;
	}
}

AProceduralSplineWall* ALabyrinthParser::SpawnWall(FVector& Location, ETravellingDirection TravellingDirection,
                                                   uint8 WallType)
{
	if (ProceduralWallClass == nullptr) return nullptr;

	AProceduralSplineWall* ProceduralSplineWallInstance = GetWorld()->SpawnActor<AProceduralSplineWall>(
		ProceduralWallClass);

	if (ProceduralSplineWallInstance == nullptr) return nullptr;

	ProceduralSplineWallInstance->SetActorScale3D(FVector(1.f, 1.f, 3.f));
	const float WallAge{FMath::RandRange(0.f, 2.f)};
	const float PillarAge{FMath::RandRange(0.f, 2.f)};
	const auto WallRandomColor = GetRandomHex(ChosenPalette);
	const auto PillarRandomColor = GetRandomHex(ChosenPalette);

	ProceduralSplineWallInstance->EditWallLook(WallType, WallRandomColor, FMath::RandBool() ? WallAge : 0.f);

	ProceduralSplineWallInstance->EditPillarLook(ChosenPillarMaterial, PillarRandomColor,
	                                             FMath::RandBool() ? PillarAge : 0.f);

	ProceduralSplineWallInstance->ClearSplinePoints();
	ProceduralSplineWallInstance->AddSplinePoint(Location);

	if (TravellingDirection == ETravellingDirection::ETD_Horizontal)
		ProceduralSplineWallInstancesHorizontal.Add(ProceduralSplineWallInstance);
	else if (TravellingDirection == ETravellingDirection::ETD_Vertical)
		ProceduralSplineWallInstancesVertical.Add(ProceduralSplineWallInstance);
	else if (TravellingDirection == ETravellingDirection::ETD_Flat)
		ProceduralSplineWallInstancesFlats.Add(ProceduralSplineWallInstance);

	return ProceduralSplineWallInstance;
}

AProceduralSplineWall* ALabyrinthParser::GetPossibleNeighborhood(FVector& Location,
                                                                 ETravellingDirection TravellingDirection) const
{
	for (auto& WallInstance : TravellingDirection == ETravellingDirection::ETD_Horizontal
		                          ? ProceduralSplineWallInstancesHorizontal
		                          : ProceduralSplineWallInstancesVertical)
	{
		if (WallInstance->GetBackwardNeighbor(Location, TravellingDirection))
			return WallInstance;
	}

	return nullptr;
}

FVector ALabyrinthParser::GetRandomHex(EWallPalette Palette)
{
	std::vector<float> ColorVec;

	switch (Palette)
	{
	case EWP_Aqua:
		ColorVec = WallSettings::AquaPalette[FMath::RandRange(0, WallSettings::AquaPalette.capacity() - 1)];
		return FVector{ColorVec[0], ColorVec[1], ColorVec[2]};
	case EWP_Autumn:
		ColorVec = WallSettings::AutumnPalette[FMath::RandRange(0, WallSettings::AutumnPalette.capacity() - 1)];
		return FVector{ColorVec[0], ColorVec[1], ColorVec[2]};
	default: ;
		return FVector{0.f, 0.f, 0.f};
	}
}

bool ALabyrinthParser::HasFrontNeighbor(uint8 Row, uint8 Column, ETravellingDirection TravellingDirection) const
{
	switch (TravellingDirection)
	{
	case ETravellingDirection::ETD_Vertical:
		if (Row == std::size(UnparsedLabyrinthMatrix) - 1) return false;
		return UnparsedLabyrinthMatrix[Row + 1][Column] != 0;
	case ETravellingDirection::ETD_Horizontal:
		if (Column == (std::size(UnparsedLabyrinthMatrix[0]) - 1)) return false;
		return UnparsedLabyrinthMatrix[Row][Column + 1] != 0;
	case ETravellingDirection::ETD_Flat:
		break;
	default: ;
	}

	return false;
}

void ALabyrinthParser::SpawnPillarAtIntersection(uint8 RowIndex, uint8 ColumnIndex, FVector& SpawnLocation,
                                                 ETravellingDirection TravellingDirection)
{
	// If there is a 1 in each direction then we should spawn a pillar
	if (RowIndex > 0 && RowIndex < (std::size(UnparsedLabyrinthMatrix) - 1)
		&& ColumnIndex > 0 && ColumnIndex < (std::size(UnparsedLabyrinthMatrix[RowIndex]) - 1) &&
		UnparsedLabyrinthMatrix[RowIndex - 1][ColumnIndex] && UnparsedLabyrinthMatrix[RowIndex + 1][ColumnIndex] &&
		UnparsedLabyrinthMatrix[RowIndex][ColumnIndex + 1]
		&& UnparsedLabyrinthMatrix[RowIndex][ColumnIndex - 1])
		SpawnWall(SpawnLocation, TravellingDirection, UnparsedLabyrinthMatrix[RowIndex][ColumnIndex]);
}

void ALabyrinthParser::SpawnFlatSurface(bool bFloor)
{
	FVector Start = ProceduralSplineWallInstancesHorizontal[0]->GetSplineComponent()->GetLocationAtSplinePoint(
		0, ESplineCoordinateSpace::World);

	Start -= FVector{bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset, 0.f, 0.f};

	FVector End = ProceduralSplineWallInstancesHorizontal[1]->GetSplineComponent()->GetLocationAtSplinePoint(
		ProceduralSplineWallInstancesHorizontal[1]->GetSplineComponent()->GetNumberOfSplinePoints(),
		ESplineCoordinateSpace::World);

	End += FVector{bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset, 0.f, 0.f};

	FVector VerticalEnd = ProceduralSplineWallInstancesVertical[0]->GetSplineComponent()->GetLocationAtSplinePoint(
		ProceduralSplineWallInstancesVertical[0]->GetSplineComponent()->GetNumberOfSplinePoints(),
		ESplineCoordinateSpace::World);

	VerticalEnd += FVector{0.f, (bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset) * 2, 0.f};

	AProceduralSplineWall* FlatWall = SpawnWall(Start, ETravellingDirection::ETD_Flat, ChosenOuterWallMaterial);

	float WallHeight = ProceduralSplineWallInstancesVertical[0]->GetActorScale3D().Z;

	if (!IsValid(FlatWall)) return;

	const auto FlatWallRandomColor = GetRandomHex(ChosenPalette);
	FlatWall->AddSplinePoint(End);
	FlatWall->AddActorWorldOffset(FVector(0.f, bFloor ? -WallSettings::FloorOffset : -WallSettings::CeilingOffset,
	                                      bFloor ? 0.f : WallHeight * WallSettings::WallHeight));

	FlatWall->SetActorRotation(FRotator(0.f, 0.f, 90.f));
	FlatWall->SetActorScale3D(FVector(1.f, 1.f, (VerticalEnd.Y - Start.Y) * 0.01f));
	FlatWall->AddPillarAtEnd(false);
	FlatWall->AddPillarAtStart(false);
	FlatWall->AddWallTop(false);
	FlatWall->EditWallLook(ChosenFlatWallMaterial, FlatWallRandomColor, false);
	ProceduralSplineWallInstancesFlats.Add(FlatWall);
}
