#include "LabyrinthParser.h"

#include "Config.h"
#include "ProceduralSplineWall.h"
#include "SceneController.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Scene_Log);

ALabyrinthParser::ALabyrinthParser()
{
	PrimaryActorTick.bCanEverTick = false;
	LabyrinthDto = nullptr;
	SpawnManager = nullptr;
}

void ALabyrinthParser::BeginPlay()
{
	Super::BeginPlay();
}

void ALabyrinthParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timer += DeltaTime;
}

FString ALabyrinthParser::BuildLabyrinth(ULabyrinthDTO* LabyrinthDtoReference, ASpawnManager* SpawnManagerReference)
{
	LabyrinthDto = LabyrinthDtoReference;
	if (LabyrinthDto->LabyrinthStructure.capacity() <= 0) return "Matrix is empty";

	const uint8 Rows = std::size(LabyrinthDto->LabyrinthStructure);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Rows: %d"), Rows);
	const uint8 Columns = std::size(LabyrinthDto->LabyrinthStructure[0]);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Columns: %d"), Columns);

	WallSettings::LabyrinthWidth = Columns;

	SpawnManager = SpawnManagerReference;
	BuildLabyrinthInternal();
	return "";
}

void ALabyrinthParser::BuildLabyrinthInternal()
{
	// Select material and color palette
	ChosenOuterWallMaterial = OuterWallMaterials[FMath::RandRange(0, OuterWallMaterials.Num() - 1)]; // Perimeter walls
	ChosenOuterPillarMaterial = OuterPillarMaterials[FMath::RandRange(0, OuterPillarMaterials.Num() - 1)]; // Outer pillars
	ChosenFlatWallMaterial = FlatWallMaterials[FMath::RandRange(0, FlatWallMaterials.Num() - 1)]; // Floor & ceiling
	ChosenPalette = static_cast<EWallPalette>(FMath::RandRange(EWallPalette::Ewp_Aqua, EWallPalette::Ewp_Reddish));

	// Create meshes
	float Offset = 0.f;

	// Add horizontal splines
	for (uint8 i = 0; i < std::size(LabyrinthDto->LabyrinthStructure); i++)
	{
		TravelHorizontal(i, std::size(LabyrinthDto->LabyrinthStructure[0]), ETravellingDirection::Etd_Horizontal, 0.f,
		                 0.f + Offset);
		Offset += WallSettings::WallOffset;
	}

	Offset = 0.f;

	// Add vertical splines
	for (uint8 i = 0; i < std::size(LabyrinthDto->LabyrinthStructure[0]); i++)
	{
		TravelVertical(i, std::size(LabyrinthDto->LabyrinthStructure), ETravellingDirection::Etd_Vertical, 0.f + Offset, 0.f);
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
	int32 SplinePoints;
	// Update meshes and look for potential "short" walls
	for (auto& ProceduralSplineWallPair : ProceduralSplineWallInstancesHorizontal)
	{
		if (ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == WallSettings::ShortWall)
			SetShortWallSettings(ProceduralSplineWallPair);

		SplinePoints = ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints();

		for(int i = 0; i < SplinePoints; i++)
		{
			if(i == 0 || i == SplinePoints - 1) continue;
			ProceduralSplineWallPair->GetSplineComponent()->RemoveSplinePoint(1);
		}
		
		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	for (auto& ProceduralSplineWallPair : ProceduralSplineWallInstancesVertical)
	{
		if (ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == WallSettings::ShortWall)
			SetShortWallSettings(ProceduralSplineWallPair);
		
		SplinePoints = ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints();
		
		for(int i = 0; i < SplinePoints; i++)
		{
			if(i == 0 || i == SplinePoints - 1) continue;
			ProceduralSplineWallPair->GetSplineComponent()->RemoveSplinePoint(1);
		}
		
		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	for (const auto& ProceduralSplineWallPair : ProceduralSplineWallInstancesFlats)
	{
		ProceduralSplineWallPair->UpdateSplineMesh();
	}
}

void ALabyrinthParser::TravelHorizontal(const uint8 RowIndex, const uint8 FinalColumnIndex,
                                        const ETravellingDirection TravellingDirection, float XOffset, const float YOffset)
{
	for (uint8 j = 0; j < FinalColumnIndex; j++)
	{
		FVector SpawnLocation = FVector(XOffset, YOffset, 0.f);

		AProceduralSplineWall* Neighbor = GetPossibleNeighborhood(SpawnLocation, TravellingDirection);

		if (LabyrinthDto->LabyrinthStructure[RowIndex][j] != 1)
		{
			SpawnManager->FindPotentialSpawnLocations(LabyrinthDto, RowIndex, j);
			XOffset += WallSettings::WallOffset;
			continue;
		}

		if (!HasFrontNeighbor(RowIndex, j, TravellingDirection) && Neighbor == nullptr || (LabyrinthDto->LabyrinthStructure[RowIndex][j] != 1))
		{
			XOffset += WallSettings::WallOffset;
			continue;
		}

		if (Neighbor == nullptr)
		{
			uint8 WallType;
			bool bIsOuterWall = false;
			if (RowIndex == 0 || RowIndex == std::size(LabyrinthDto->LabyrinthStructure) - 1)
			{
				WallType = ChosenOuterWallMaterial;
				bIsOuterWall = true;
			}
			else
			{
				WallType = InsideWallMaterials[FMath::RandRange(0, InsideWallMaterials.Num() - 1)];
			}

			SpawnWall(SpawnLocation, TravellingDirection, WallType, bIsOuterWall);
		}
		else
		{
			Neighbor->AddSplinePoint(SpawnLocation);
		}

		SpawnPillarAtIntersection(RowIndex, j, SpawnLocation, TravellingDirection);

		XOffset += WallSettings::WallOffset;
	}
}

void ALabyrinthParser::TravelVertical(const uint8 ColumnIndex, const uint8 FinalRowIndex, const ETravellingDirection TravellingDirection,
                                      const float XOffset, float YOffset)
{
	for (uint8 i = 0; i < FinalRowIndex; i++)
	{
		FVector SpawnLocation = FVector(XOffset, YOffset, 0.f);

		AProceduralSplineWall* Neighbor = GetPossibleNeighborhood(SpawnLocation, TravellingDirection);

		if (LabyrinthDto->LabyrinthStructure[i][ColumnIndex] != 1 || (!HasFrontNeighbor(i, ColumnIndex, TravellingDirection) &&
			Neighbor == nullptr))
		{
			YOffset += WallSettings::WallOffset;
			continue;
		}

		if (Neighbor == nullptr)
		{
			uint8 WallType;
			bool bIsOuterWall = false;
			if (ColumnIndex == 0 || ColumnIndex == std::size(LabyrinthDto->LabyrinthStructure[0]) - 1)
			{
				WallType = ChosenOuterWallMaterial;
				bIsOuterWall = true;
			}
            else
            {
	            WallType = InsideWallMaterials[FMath::RandRange(0, InsideWallMaterials.Num() - 1)];
            }

			SpawnWall(SpawnLocation, TravellingDirection, WallType, bIsOuterWall);
		}
		else
			Neighbor->AddSplinePoint(SpawnLocation);

		YOffset += WallSettings::WallOffset;
	}
}

void ALabyrinthParser::SetShortWallSettings(AProceduralSplineWall* & ProceduralSplineWall)
{
	const auto WallRandomColor = GetRandomColor(ChosenPalette);
	const auto PillarRandomColor = GetRandomColor(ChosenPalette);
	const float WallAge{FMath::RandRange(0.f, 1.f)};
	const float PillarAge{FMath::RandRange(0.f, 1.f)};
	const auto ShortWallMaterial{ShortWallMaterials[FMath::RandRange(0, ShortWallMaterials.Num() - 1)]};
	
	ProceduralSplineWall->EditWallLook(ShortWallMaterial, WallRandomColor, WallAge);

	ProceduralSplineWall->EditPillarLook(ShortWallMaterial, PillarRandomColor, PillarAge);
}

AProceduralSplineWall* ALabyrinthParser::SpawnWall(FVector& Location, const ETravellingDirection TravellingDirection,
                                                   const uint8 WallType, const bool bIsOuterWall)
{
	if (!(IsValid(ProceduralWallClass))) return nullptr;

	AProceduralSplineWall* ProceduralSplineWallInstance = GetWorld()->SpawnActor<AProceduralSplineWall>(
		ProceduralWallClass);

	if (ProceduralSplineWallInstance == nullptr) return nullptr;

	ProceduralSplineWallInstance->SetActorScale3D(FVector(1.f, 1.f, 3.f));

	float WallAge;
	float PillarAge;
	FVector WallRandomColor;
	FVector PillarRandomColor;
	uint8 PillarMaterial = InsidePillarMaterials[FMath::RandRange(0, InsidePillarMaterials.Num() - 1)];
	if (bIsOuterWall)
	{
		WallAge = 0.5f;
		PillarAge = 0.4f;
		WallRandomColor = GetRandomColorForOuterWall(ChosenPalette);
		PillarRandomColor = GetRandomColorForOuterPillar(ChosenPalette);
		PillarMaterial = ChosenOuterPillarMaterial;
	}
	else
	{
		WallAge = FMath::RandRange(0.f, 1.f);
		PillarAge = FMath::RandRange(0.f, 1.f);
		WallRandomColor = GetRandomColor(ChosenPalette);
		PillarRandomColor = GetRandomColor(ChosenPalette);
	}

	ProceduralSplineWallInstance->EditWallLook(WallType, WallRandomColor, WallAge);

	ProceduralSplineWallInstance->EditPillarLook(PillarMaterial, PillarRandomColor, PillarAge);

	ProceduralSplineWallInstance->ClearSplinePoints();
	ProceduralSplineWallInstance->AddSplinePoint(Location);

	if (TravellingDirection == ETravellingDirection::Etd_Horizontal)
		ProceduralSplineWallInstancesHorizontal.Add(ProceduralSplineWallInstance);
	else if (TravellingDirection == ETravellingDirection::Etd_Vertical)
		ProceduralSplineWallInstancesVertical.Add(ProceduralSplineWallInstance);
	else if (TravellingDirection == ETravellingDirection::Etd_Flat)
		ProceduralSplineWallInstancesFlats.Add(ProceduralSplineWallInstance);

	return ProceduralSplineWallInstance;
}

AProceduralSplineWall* ALabyrinthParser::GetPossibleNeighborhood(FVector& Location,
                                                                 const ETravellingDirection TravellingDirection) const
{
	for (auto& WallInstance : TravellingDirection == ETravellingDirection::Etd_Horizontal
		                          ? ProceduralSplineWallInstancesHorizontal
		                          : ProceduralSplineWallInstancesVertical)
	{
		if (WallInstance->GetBackwardNeighbor(Location, TravellingDirection))
			return WallInstance;
	}

	return nullptr;
}

FVector ALabyrinthParser::GetRandomColor(const EWallPalette Palette)
{
	std::vector<float> ColorVector;

	switch (Palette)
	{
	case Ewp_Aqua:
		ColorVector = WallSettings::AquaPalette[FMath::RandRange(0, WallSettings::AquaPalette.capacity() - 1)];
		return FVector{ColorVector[0], ColorVector[1], ColorVector[2]};
	case Ewp_Autumn:
		ColorVector = WallSettings::AutumnPalette[FMath::RandRange(0, WallSettings::AutumnPalette.capacity() - 1)];
		return FVector{ColorVector[0], ColorVector[1], ColorVector[2]};
	case Ewp_Rose:
		ColorVector = WallSettings::RosePalette[FMath::RandRange(0, WallSettings::RosePalette.capacity() - 1)];
		return FVector{ColorVector[0], ColorVector[1], ColorVector[2]};
	case Ewp_BlueYellow:
		ColorVector = WallSettings::BluYellowPalette[FMath::RandRange(0, WallSettings::BluYellowPalette.capacity() - 1)];
		return FVector{ColorVector[0], ColorVector[1], ColorVector[2]};
	case Ewp_Reddish:
		ColorVector = WallSettings::ReddishPalette[FMath::RandRange(0, WallSettings::ReddishPalette.capacity() - 1)];
		return FVector{ColorVector[0], ColorVector[1], ColorVector[2]};
	default: ;
		return FVector{0.f, 0.f, 0.f};
	}
}

FVector ALabyrinthParser::GetRandomColorForOuterWall(const EWallPalette Palette)
{
	if (OuterWallColor.IsZero())
		OuterWallColor = GetRandomColor(Palette);
	return OuterWallColor;
}

FVector ALabyrinthParser::GetRandomColorForOuterPillar(const EWallPalette Palette)
{	
	if (OuterPillarColor.IsZero())
		OuterPillarColor = GetRandomColor(Palette);
	return OuterPillarColor;
}

bool ALabyrinthParser::HasFrontNeighbor(const uint8 Row, const uint8 Column, const ETravellingDirection TravellingDirection) const
{
	switch (TravellingDirection)
	{
	case ETravellingDirection::Etd_Vertical:
		if (Row == std::size(LabyrinthDto->LabyrinthStructure) - 1) return false;
		return LabyrinthDto->LabyrinthStructure[Row + 1][Column] == 1;
	case ETravellingDirection::Etd_Horizontal:
		if (Column == (std::size(LabyrinthDto->LabyrinthStructure[0]) - 1)) return false;
		return LabyrinthDto->LabyrinthStructure[Row][Column + 1] == 1;
	case ETravellingDirection::Etd_Flat:
		break;
	default: ;
	}

	return false;
}

void ALabyrinthParser::SpawnPillarAtIntersection(const uint8 RowIndex, const uint8 ColumnIndex, FVector& SpawnLocation,
                                                 const ETravellingDirection TravellingDirection)
{
	// If there is a 1 in each direction then we should spawn a pillar
	if (RowIndex > 0 && RowIndex < (std::size(LabyrinthDto->LabyrinthStructure) - 1)
		&& ColumnIndex > 0 && ColumnIndex < (std::size(LabyrinthDto->LabyrinthStructure[RowIndex]) - 1) &&
		LabyrinthDto->LabyrinthStructure[RowIndex - 1][ColumnIndex] == 1 && LabyrinthDto->LabyrinthStructure[RowIndex + 1][ColumnIndex] == 1 &&
		LabyrinthDto->LabyrinthStructure[RowIndex][ColumnIndex + 1] == 1
		&& LabyrinthDto->LabyrinthStructure[RowIndex][ColumnIndex - 1] == 1)
		SpawnWall(SpawnLocation, TravellingDirection, LabyrinthDto->LabyrinthStructure[RowIndex][ColumnIndex]);
}

void ALabyrinthParser::SpawnFlatSurface(const bool bFloor)
{
	FVector Start = FVector{0.f, 0.f, 0.f};

	Start -= FVector{bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset, 0.f, 0.f};

	FVector End{std::size(LabyrinthDto->LabyrinthStructure[0]) * WallSettings::WallOffset, 0.f, 0.f};

	End += FVector{(bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset) - WallSettings::WallOffset, 0.f, 0.f};

	FVector VerticalEnd = {0.f, std::size(LabyrinthDto->LabyrinthStructure) * WallSettings::WallOffset, 0.f};

	VerticalEnd += FVector{0.f, (bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset) * 2 - WallSettings::WallOffset, 0.f};

	AProceduralSplineWall* FlatWall = SpawnWall(Start, ETravellingDirection::Etd_Flat, ChosenOuterWallMaterial);

	float WallHeight = ProceduralSplineWallInstancesVertical[0]->GetActorScale3D().Z;

	if (!IsValid(FlatWall)) return;

	const auto FlatWallRandomColor = GetRandomColor(ChosenPalette);
	FlatWall->AddSplinePoint(End);
	FlatWall->AddActorWorldOffset(FVector(0.f, bFloor ? -WallSettings::FloorOffset : -WallSettings::CeilingOffset,
	                                      bFloor ? 0.f : WallHeight * WallSettings::WallHeight));

	FlatWall->SetActorRotation(FRotator(0.f, 0.f, 90.f));
	FlatWall->SetActorScale3D(FVector(1.f, 1.f, (VerticalEnd.Y - Start.Y) * 0.01f));
	FlatWall->AddPillarAtEnd(false);
	FlatWall->AddPillarAtStart(false);
	FlatWall->AddWallTop(false);
	FlatWall->EditWallLook(ChosenFlatWallMaterial, FlatWallRandomColor, false);
	FlatWall->SetCollision(true);
	ProceduralSplineWallInstancesFlats.Add(FlatWall);
}
