#include "LabyrinthParser.h"

#include "Config.h"
#include "ProceduralSplineWall.h"
#include "SceneController.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Scene_Log);

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
	Timer += DeltaTime;
}

bool ALabyrinthParser::BuildLabyrinth(const std::vector<std::vector<uint8>>& Matrix)
{
	if (Matrix.capacity() <= 0) return false;

	const uint8 Rows = std::size(Matrix);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Rows: %d"), Rows);	
	const uint8 Columns = std::size(Matrix[0]);
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("Columns: %d"), Columns);

	UnparsedLabyrinthMatrix.resize(Rows, std::vector<uint8>(Columns));

	for (uint8 i = 0; i < Rows; i++)
	{
		for (uint8 j = 0; j < Columns; j++)
		{
			UnparsedLabyrinthMatrix[i][j] = Matrix[i][j];
		}
	}
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("UnparsedLabyrinthMatrix Rows: %llu"), std::size(UnparsedLabyrinthMatrix));
	UE_LOG(LabyrAInthVR_Scene_Log, Display, TEXT("UnparsedLabyrinthMatrix Columns: %llu"), std::size(UnparsedLabyrinthMatrix[0]));

	BuildLabyrinthInternal();

	return true;
}

FVector ALabyrinthParser::GetNextDestination(uint8& Row, uint8& Column, EEnemyDirection& LastDirection)
{
	EEnemyDirection ChosenDirection{EED_None};
	TArray<EEnemyDirection> FreeEnemyDirections{};
	bool bIsInRoom{false};
	// Get all free roaming directions
	FillFreeDirections(Row, Column, FreeEnemyDirections);
	bIsInRoom = IsInRoom(Row, Column, FreeEnemyDirections);
	// Check if is in room
	if (bIsInRoom)
	{
		FreeEnemyDirections.Remove(EED_Diagonal);
		UE_LOG(LogTemp, Error, TEXT("The enemy is standing at the start of a room"))
	}

	// Pick direction based on the last known direction
	switch (LastDirection)
	{
	case EED_None:
		ChosenDirection = FreeEnemyDirections.Num() > 0
			                  ? FreeEnemyDirections[FMath::RandRange(0, FreeEnemyDirections.Num() - 1)]
			                  : EED_None;
		break;
	case EED_Left:
	case EED_Right:
		ChooseNextDirection(FreeEnemyDirections, ChosenDirection, LastDirection, 2, FreeEnemyDirections.Num() - 1);
		break;
	case EED_Down:
	case EED_Up:
		ChooseNextDirection(FreeEnemyDirections, ChosenDirection, LastDirection, 0, 1);
		break;
	case EED_Diagonal:
		ChooseNextDirection(FreeEnemyDirections, ChosenDirection, LastDirection, 0, FreeEnemyDirections.Num() - 1);
	default: ;
	}

	UE_LOG(LogTemp, Warning, TEXT("Chosen direction: %s"), *UEnum::GetValueAsString(ChosenDirection))

	// Find the (X,Y) for the chosen direction, stop at intersections
	uint8 TravellingRow{Row};
	uint8 TravellingColumn{Column};

	UE_LOG(LogTemp, Warning, TEXT("Current enemy indexes: %d %d"), TravellingRow, TravellingColumn)

	switch (ChosenDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		while (TravellingColumn - 1 >= 0 && UnparsedLabyrinthMatrix[TravellingRow][TravellingColumn - 1] == 0)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			--TravellingColumn;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Right:
		while (TravellingColumn + 1 < std::size(UnparsedLabyrinthMatrix[TravellingRow]) && UnparsedLabyrinthMatrix[
			TravellingRow][TravellingColumn + 1] == 0)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			++TravellingColumn;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Up:
		while (TravellingRow - 1 >= 0 && UnparsedLabyrinthMatrix[TravellingRow - 1][TravellingColumn] == 0)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			--TravellingRow;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Down:
		while (TravellingRow + 1 < std::size(UnparsedLabyrinthMatrix) && UnparsedLabyrinthMatrix[TravellingRow + 1][
			TravellingColumn] == 0)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			++TravellingRow;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Diagonal:
		FillDiagonalIndexes(TravellingRow, TravellingColumn);
		break;
	default: ;
	}

	const FVector Destination{
		WallSettings::WallOffset * TravellingColumn, WallSettings::WallOffset * TravellingRow, 0.f
	};

	DrawDebugSphere(GetWorld(), Destination, 20.f, 10, FColor::Red, true);
	UE_LOG(LogTemp, Warning, TEXT("Post enemy indexes: %d %d"), TravellingRow, TravellingColumn)
	// Update enemy values by ref
	LastDirection = ChosenDirection;
	Row = TravellingRow;
	Column = TravellingColumn;

	return Destination;
}

void ALabyrinthParser::FillFreeDirections(uint8 Row, uint8 Column, TArray<EEnemyDirection>& FreeEnemyDirections)
{
	if (Column - 1 >= 0 && UnparsedLabyrinthMatrix[Row][Column - 1] == 0)
		FreeEnemyDirections.Add(
			EEnemyDirection::EED_Left);
	if (Column + 1 < std::size(UnparsedLabyrinthMatrix[Row]) && UnparsedLabyrinthMatrix[Row][Column + 1] == 0)
		FreeEnemyDirections.Add(EEnemyDirection::EED_Right);
	if (Row + 1 < std::size(UnparsedLabyrinthMatrix) && UnparsedLabyrinthMatrix[Row + 1][Column] == 0)
		FreeEnemyDirections.Add(EEnemyDirection::EED_Down);
	if (Row - 1 >= 0 && UnparsedLabyrinthMatrix[Row - 1][Column] == 0) FreeEnemyDirections.Add(EEnemyDirection::EED_Up);


	const bool bDiagonal = IsDiagonal(Row, Column);
	UE_LOG(LogTemp, Warning, TEXT("Diagonal presence: %s"), bDiagonal ? *FString("True") : *FString("False"))

	if (bDiagonal) FreeEnemyDirections.Add(EEnemyDirection::EED_Diagonal);
}

void ALabyrinthParser::ChooseNextDirection(TArray<EEnemyDirection>& EnemyDirections, EEnemyDirection& NextDirection,
                                           EEnemyDirection PreviousDirection, uint8 MinIndex, uint8 MaxIndex)
{
	const float SwitchAtIntersecValue = FMath::RandRange(0.f, 1.f);
	const float SwitchToDiagonalValue = FMath::RandRange(0.f, 1.f);

	for (const auto& RandomDirection : EnemyDirections)
		UE_LOG(LogTemp, Warning, TEXT("Free direction: %s"), *UEnum::GetValueAsString(RandomDirection))

	UE_LOG(LogTemp, Warning, TEXT("Previous direction: %s"), *UEnum::GetValueAsString(PreviousDirection))
	// Pick diagonal first
	if (EnemyDirections.Contains(EED_Diagonal) && SwitchToDiagonalValue < EnemySettings::TurnAtDiagonalProbability &&
		PreviousDirection != EED_Diagonal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Switching to diagonal: %f < %f"), SwitchToDiagonalValue,
		       EnemySettings::TurnAtDiagonalProbability)
		NextDirection = EED_Diagonal;
		return;
	}

	if (EnemyDirections.Num() > 1) EnemyDirections.Remove(EED_Diagonal);

	// No free directions, we're stuck 
	if (EnemyDirections.Num() <= 0)
	{
		NextDirection = EED_None;
		return;
	}

	// Blind spot, only way is to go back
	if (EnemyDirections.Num() == 1)
	{
		NextDirection = EnemyDirections[0];
		return;
	}

	// It means that I stopped in a corner
	if (EnemyDirections.Num() >= 2 || SwitchAtIntersecValue < EnemySettings::TurnAtIntersecProbability ||
		PreviousDirection == EED_Diagonal)
	{
		NextDirection = GetIntersecDirection(EnemyDirections, PreviousDirection);
		return;
	}

	NextDirection = PreviousDirection;
}

EEnemyDirection ALabyrinthParser::GetOppositeDirection(EEnemyDirection EnemyDirection)
{
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		return EED_Right;
	case EED_Right:
		return EED_Left;
	case EED_Up:
		return EED_Down;
	case EED_Down:
		return EED_Up;
	case EED_Diagonal:
		return EED_Diagonal;
	default: ;
	}

	return EED_None;
}

EEnemyDirection ALabyrinthParser::GetIntersecDirection(TArray<EEnemyDirection>& EnemyDirections,
                                                       EEnemyDirection EnemyDirection)
{
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
	case EED_Right:
		if (EnemyDirections.Contains(EED_Up)) return EED_Up;
		if (EnemyDirections.Contains(EED_Down)) return EED_Down;
		break;
	case EED_Up:
	case EED_Down:
		if (EnemyDirections.Contains(EED_Right)) return EED_Right;
		if (EnemyDirections.Contains(EED_Left)) return EED_Left;
		break;
	case EED_Diagonal:
		return EnemyDirections[FMath::RandRange(0, EnemyDirections.Num() - 1)];
	default: ;
	}
	return EED_None;
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
	{
		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	// Enemy spawn
	bool bEnemySpawnPointFound = false;
	FVector SpawnPoint{0};
	uint8 RowSelection{0};
	uint8 ColumnSelection{0};
	while (!bEnemySpawnPointFound)
	{
		RowSelection = FMath::RandRange(0, std::size(UnparsedLabyrinthMatrix) - 1);
		ColumnSelection = FMath::RandRange(0, std::size(UnparsedLabyrinthMatrix[0]) - 1);

		if (!UnparsedLabyrinthMatrix[RowSelection][ColumnSelection])
		{
			bEnemySpawnPointFound = true;
			SpawnPoint = FVector{
				WallSettings::WallOffset * ColumnSelection, WallSettings::WallOffset * RowSelection,
				EnemySettings::SpawnHeight
			};
			//DrawDebugSphere(GetWorld(), SpawnPoint, 20.f, 15, FColor::Red, true);
		}
	}

	if (BaseEnemyClass == nullptr) return;

	ABaseEnemy* EnemyInstance = GetWorld()->SpawnActor<ABaseEnemy>(BaseEnemyClass, SpawnPoint, FRotator{0.f, 0.f, 0.f});

	if (EnemyInstance == nullptr) return;

	EnemyInstance->SetOwner(this);
	EnemyInstance->SetMatrixPosition(RowSelection, ColumnSelection);
	SpawnedEnemies.Add(EnemyInstance);

	// Broadcast scene complete
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
	if (!(IsValid(ProceduralWallClass))) return nullptr;

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

bool ALabyrinthParser::IsIntersection(uint8 Row, uint8 Column) const
{
	uint8 Counter{0};
	if (Column + 1 < std::size(UnparsedLabyrinthMatrix[Row]) && UnparsedLabyrinthMatrix[Row][Column + 1] == 0)
		Counter
			++;
	if (Column - 1 >= 0 && UnparsedLabyrinthMatrix[Row][Column - 1] == 0) Counter++;
	if (Row + 1 < std::size(UnparsedLabyrinthMatrix) && UnparsedLabyrinthMatrix[Row + 1][Column] == 0) Counter++;
	if (Row - 1 >= 0 && UnparsedLabyrinthMatrix[Row - 1][Column] == 0) Counter++;

	return Counter > 2;
}

bool ALabyrinthParser::IsDiagonal(uint8 Row, uint8 Column) const
{
	if (Column - 1 >= 0 && Row - 1 >= 0 && UnparsedLabyrinthMatrix[Row - 1][Column - 1] == 0) return true;
	if (Column + 1 < std::size(UnparsedLabyrinthMatrix[Row]) && Row - 1 >= 0 && UnparsedLabyrinthMatrix[Row - 1][Column
		+ 1] == 0)
		return true;
	if (Column - 1 >= 0 && Row + 1 < std::size(UnparsedLabyrinthMatrix) && UnparsedLabyrinthMatrix[Row + 1][Column - 1]
		== 0)
		return true;
	if (Column + 1 < std::size(UnparsedLabyrinthMatrix[Row]) && Row + 1 < std::size(UnparsedLabyrinthMatrix) &&
		UnparsedLabyrinthMatrix[Row + 1][Column + 1] == 0)
		return true;

	return false;
}

bool ALabyrinthParser::IsInRoom(uint8 Row, uint8 Column, const TArray<EEnemyDirection>& FreeEnemyDirections)
{
	TArray<EEnemyDiagonalDirection> EnemyDiagonalDirections;
	FillDiagonalMatrix(Row, Column, EnemyDiagonalDirections);

	for (const auto& DiagonalDirection : EnemyDiagonalDirections)
	{
		switch (DiagonalDirection)
		{
		case EEDD_UpperRight:
			if (FreeEnemyDirections.Contains(EED_Up) && FreeEnemyDirections.Contains(EED_Right)) return true;
			break;
		case EEDD_UpperLeft:
			if (FreeEnemyDirections.Contains(EED_Up) && FreeEnemyDirections.Contains(EED_Left)) return true;
			break;
		case EEDD_LowerRight:
			if (FreeEnemyDirections.Contains(EED_Down) && FreeEnemyDirections.Contains(EED_Right)) return true;
			break;
		case EEDD_LowerLeft:
			if (FreeEnemyDirections.Contains(EED_Down) && FreeEnemyDirections.Contains(EED_Left)) return true;
			break;
		default: ;
		}
	}

	return false;
}

bool ALabyrinthParser::CheckForExit(uint8 Row, uint8 Column, EEnemyDirection EnemyDirection)
{
	UE_LOG(LogTemp, Warning, TEXT("Checking for exit in: %s Row: %d Column: %d"),
	       *UEnum::GetValueAsString(EnemyDirection), Row, Column);
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		if (UnparsedLabyrinthMatrix[Row - 1][Column] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column - 1] == 0)
			return
				true;
		if (UnparsedLabyrinthMatrix[Row + 1][Column] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column - 1] == 0)
			return
				true;
		break;
	case EED_Right:
		if (UnparsedLabyrinthMatrix[Row - 1][Column] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column + 1] == 0)
			return
				true;
		if (UnparsedLabyrinthMatrix[Row + 1][Column] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column + 1] == 0)
			return
				true;
		break;
	case EED_Up:
		if (UnparsedLabyrinthMatrix[Row][Column + 1] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column + 1] == 0)
			return
				true;
		if (UnparsedLabyrinthMatrix[Row][Column - 1] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column - 1] == 0)
			return
				true;
		break;
	case EED_Down:
		if (UnparsedLabyrinthMatrix[Row][Column + 1] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column + 1] == 0)
			return
				true;
		if (UnparsedLabyrinthMatrix[Row][Column - 1] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column - 1] == 0)
			return
				true;
		break;
	case EED_Diagonal:
		break;
	default: ;
	}

	return false;
}

void ALabyrinthParser::FillExitIndexes(uint8& Row, uint8& Column, EEnemyDirection EnemyDirection)
{
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		if (UnparsedLabyrinthMatrix[Row - 1][Column] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column - 1] == 0)
		{
			Row--;
			Column--;
			break;
		}
		if (UnparsedLabyrinthMatrix[Row + 1][Column] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column - 1] == 0)
		{
			Row++;
			Column--;
			break;
		}
		break;
	case EED_Right:
		if (UnparsedLabyrinthMatrix[Row - 1][Column] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column + 1] == 0)
		{
			Row--;
			Column++;
			break;
		}
		if (UnparsedLabyrinthMatrix[Row + 1][Column] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column + 1] == 0)
		{
			Row++;
			Column++;
			break;
		}
		break;
	case EED_Up:
		if (UnparsedLabyrinthMatrix[Row][Column + 1] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column + 1] == 0)
		{
			Row--;
			Column++;
			break;
		}
		if (UnparsedLabyrinthMatrix[Row][Column - 1] == 1 && UnparsedLabyrinthMatrix[Row - 1][Column - 1] == 0)
		{
			Row--;
			Column--;
			break;
		}
		break;
	case EED_Down:
		if (UnparsedLabyrinthMatrix[Row][Column + 1] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column + 1] == 0)
		{
			Row++;
			Column++;
			break;
		}
		if (UnparsedLabyrinthMatrix[Row][Column - 1] == 1 && UnparsedLabyrinthMatrix[Row + 1][Column - 1] == 0)
		{
			Row++;
			Column--;
			break;
		}
		break;
	case EED_Diagonal:
		break;
	default: ;
	}
}

void ALabyrinthParser::FillDiagonalIndexes(uint8& Row, uint8& Column)
{
	UE_LOG(LogTemp, Warning, TEXT("Filling diagonal indexes"))

	TArray<EEnemyDiagonalDirection> DiagonalDirections;

	FillDiagonalMatrix(Row, Column, DiagonalDirections);

	switch (DiagonalDirections[FMath::RandRange(0, DiagonalDirections.Num() - 1)])
	{
	case EEDD_UpperRight:
		Row--;
		Column++;
		break;
	case EEDD_UpperLeft:
		Row--;
		Column--;
		break;
	case EEDD_LowerRight:
		Row++;
		Column++;
		break;
	case EEDD_LowerLeft:
		Row++;
		Column--;
		break;
	default: ;
	}
}

void ALabyrinthParser::FillDiagonalMatrix(uint8 Row, uint8 Column,
                                          TArray<EEnemyDiagonalDirection>& EnemyDiagonalDirections)
{
	if (Column - 1 >= 0 && Row - 1 >= 0 && UnparsedLabyrinthMatrix[Row - 1][Column - 1] == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Upper left"))
		EnemyDiagonalDirections.Add(EEDD_UpperLeft);
	}
	if (Column + 1 < std::size(UnparsedLabyrinthMatrix[Row]) && Row - 1 >= 0 && UnparsedLabyrinthMatrix[Row - 1][Column
		+ 1] == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Upper right"))
		EnemyDiagonalDirections.Add(EEDD_UpperRight);
	}
	if (Column - 1 >= 0 && Row + 1 < std::size(UnparsedLabyrinthMatrix) && UnparsedLabyrinthMatrix[Row + 1][Column - 1]
		== 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom left"))
		EnemyDiagonalDirections.Add(EEDD_LowerLeft);
	}
	if (Column + 1 < std::size(UnparsedLabyrinthMatrix[Row]) && Row + 1 < std::size(UnparsedLabyrinthMatrix) &&
		UnparsedLabyrinthMatrix[Row + 1][Column + 1] == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom right"))
		EnemyDiagonalDirections.Add(EEDD_LowerRight);
	}
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
	FVector Start = FVector{0.f, 0.f, 0.f};

	Start -= FVector{bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset, 0.f, 0.f};

	/*FVector End = ProceduralSplineWallInstancesHorizontal[1]->GetSplineComponent()->GetLocationAtSplinePoint(
		ProceduralSplineWallInstancesHorizontal[1]->GetSplineComponent()->GetNumberOfSplinePoints(),
		ESplineCoordinateSpace::World);*/
	FVector End{std::size(UnparsedLabyrinthMatrix[0]) * WallSettings::WallOffset, 0.f, 0.f};

	End += FVector{bFloor ? WallSettings::FloorOffset : WallSettings::CeilingOffset, 0.f, 0.f};

	FVector VerticalEnd = {0.f, std::size(UnparsedLabyrinthMatrix) * WallSettings::WallOffset, 0.f};

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
	FlatWall->SetCollision(bFloor);
	ProceduralSplineWallInstancesFlats.Add(FlatWall);
}
