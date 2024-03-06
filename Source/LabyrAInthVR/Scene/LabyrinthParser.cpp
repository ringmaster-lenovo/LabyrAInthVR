#include "LabyrinthParser.h"

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
	return;
	ChosenOuterWallMaterial = OuterWallMaterials[FMath::RandRange(0, OuterWallMaterials.Num() - 1)];
	ChosenInsideWallMaterial = InsideWallMaterials[FMath::RandRange(0, InsideWallMaterials.Num() - 1)];
	
#pragma region PREPROCESSING
	/*ParsedLabyrinthMatrix[12][12] = { };

	for(uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix); i++)
	{
		for(uint8 j = 0; j < std::size(UnparsedLabyrinthMatrix[i]); j++)
		{
			if(!UnparsedLabyrinthMatrix[i][j]) continue;
			uint8 CurrentWallTypeValue = 0;
			
			if(j + 1 < std::size(UnparsedLabyrinthMatrix[i]) && UnparsedLabyrinthMatrix[i][j+1]) CurrentWallTypeValue += static_cast<uint8>(EWallType::EWT_HalfEast);
			if(i - 1 >= 0 && UnparsedLabyrinthMatrix[i-1][j]) CurrentWallTypeValue += static_cast<uint8>(EWallType::EWT_HalfNorth);
			if(j - 1 >= 0 && UnparsedLabyrinthMatrix[i][j-1]) CurrentWallTypeValue += static_cast<uint8>(EWallType::EWT_HalfOvest);
			if(i + 1 < std::size(UnparsedLabyrinthMatrix) && UnparsedLabyrinthMatrix[i+1][j]) CurrentWallTypeValue += static_cast<uint8>(EWallType::EWT_HalfSouth);

			EWallType CurrentWallType = static_cast<EWallType>(CurrentWallTypeValue);

			ParsedLabyrinthMatrix[i][j] = CurrentWallType;
		}
	}
	
	for(uint8 i = 0; i < std::size(ParsedLabyrinthMatrix); i++)
	{
		FString RowText = "";
		for(uint8 j = 0; j < std::size(ParsedLabyrinthMatrix[i]); j++)
		{
			RowText.Append(FString::Printf(TEXT(" %s "), *(UEnum::GetValueAsString(ParsedLabyrinthMatrix[i][j]).Replace(TEXT("EWallType::EWT_"), TEXT("")))));
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *RowText)
	}*/
#pragma endregion

#pragma region CREATE LABYRINTH MESHES
	float Offset = 0.f;

	// Add horizontal splines
	for(uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix); i++)
	{
		TravelHorizontal(i, std::size(UnparsedLabyrinthMatrix[0]), ETravellingDirection::ETD_Horizontal, 0.f, 0.f + Offset);
		Offset += 500.f;
	}

	Offset = 0.f;

	// Add vertical splines
	for(uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix[0]); i++)
	{
		TravelVertical(i, std::size(UnparsedLabyrinthMatrix), ETravellingDirection::ETD_Vertical, 0.f + Offset, 0.f);
		Offset += 500.f;
	}

	// Add floor & ceiling
	SpawnFlatSurface(false);
	SpawnFlatSurface(true);
	
	// Remove overlapping pillars
	for(auto& HorizontalSplineWall : ProceduralSplineWallInstancesHorizontal)
	{
		FVector HorizontalStartingPointLocation = HorizontalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector HorizontalEndingPointLocation = HorizontalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(HorizontalSplineWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

		//DrawDebugSphere(GetWorld(), HorizontalStartingPointLocation, 25.f, 20, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), HorizontalEndingPointLocation, 25.f, 20, FColor::Red, true);
		
		for(auto& VerticalSplineWall : ProceduralSplineWallInstancesVertical)
		{
			FVector VerticalStartingPointLocation = VerticalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
			FVector VerticalEndingPointLocation = VerticalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(VerticalSplineWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

			if (HorizontalStartingPointLocation == VerticalStartingPointLocation || HorizontalStartingPointLocation == VerticalEndingPointLocation)
			{
				HorizontalSplineWall->AddPillarAtStart(false);
				//DrawDebugSphere(GetWorld(), HorizontalStartingPointLocation, 25.f, 20, FColor::Red, true);
			}

			if(HorizontalEndingPointLocation == VerticalEndingPointLocation || HorizontalEndingPointLocation == VerticalStartingPointLocation)
			{
				HorizontalSplineWall->AddPillarAtEnd(false);
				//DrawDebugSphere(GetWorld(), HorizontalEndingPointLocation, 25.f, 20, FColor::Red, true);
			}
		}
	}
	
#pragma endregion CREATE LABYRINTH MESHES
	
	for(const auto& ProceduralSplineWallPair: ProceduralSplineWallInstancesHorizontal)
	{
		if(ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == 3)
			ProceduralSplineWallPair->EditWallLook(13);
		
		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	for(const auto& ProceduralSplineWallPair: ProceduralSplineWallInstancesVertical)
	{
		if(ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == 3)
			ProceduralSplineWallPair->EditWallLook(13);
		ProceduralSplineWallPair->UpdateSplineMesh();
	}
	
	for(const auto& ProceduralSplineWallPair: ProceduralSplineWallInstancesFlats)
		ProceduralSplineWallPair->UpdateSplineMesh();
}

void ALabyrinthParser::BuildLabyrinth(uint8 *MatrixPointer, uint8 Rows, uint8 Columns)
{
	UnparsedLabyrinthMatrix.resize(Rows, std::vector<uint8>(Columns));
	
	for(uint8 i = 0; i < Rows; i++)
	{
		for(uint8 j = 0; j < Columns; j++)
		{
			UnparsedLabyrinthMatrix[i][j] = *(MatrixPointer + i * Columns + j);
		}
	}
	
	for(uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix); i++)
	{
		for(uint8 j = 0; j < std::size(UnparsedLabyrinthMatrix[0]); j++)
		{
			//UE_LOG(LogTemp, Warning, TEXT("%d"), UnparsedLabyrinthMatrix[i][j]);
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Rows: %llu - Cols: %llu"), std::size(UnparsedLabyrinthMatrix), std::size(UnparsedLabyrinthMatrix[0]))
	
	BuildLabyrinthInternal();
}

void ALabyrinthParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALabyrinthParser::TravelHorizontal(uint8 RowIndex, uint8 FinalColumnIndex, ETravellingDirection TravellingDirection, float XOffset, float YOffset)
{
		for(uint8 j = 0; j < FinalColumnIndex; j++)
		{
			FVector SpawnLocation = FVector(XOffset, YOffset, 0.f);
			
			AProceduralSplineWall* Neighbor = GetPossibleNeighborhood(SpawnLocation, TravellingDirection);
			
			if(UnparsedLabyrinthMatrix[RowIndex][j] == 0 || (!HasFrontNeighbor(RowIndex, j, TravellingDirection) && Neighbor == nullptr))
			{
				XOffset += 500.f;
				continue;
			}
			
			if(Neighbor == nullptr)
			{
				uint8 WallType = (RowIndex == 0 || RowIndex == std::size(UnparsedLabyrinthMatrix) - 1) ? ChosenOuterWallMaterial : ChosenInsideWallMaterial;
				SpawnWall(SpawnLocation, TravellingDirection, WallType);
			}
			else Neighbor->AddSplinePoint(SpawnLocation);

			//SpawnCrossIntersection(RowIndex, j, SpawnLocation, TravellingDirection);

			XOffset += 500.f;
		}
}

void ALabyrinthParser::TravelVertical(uint8 ColumnIndex, uint8 FinalRowIndex, ETravellingDirection TravellingDirection, float XOffset, float YOffset)
{
		for(uint8 i = 0; i < FinalRowIndex; i++)
		{
			FVector SpawnLocation = FVector(XOffset, YOffset, 0.f);
			
			AProceduralSplineWall* Neighbor = GetPossibleNeighborhood(SpawnLocation, TravellingDirection);
			
			if(UnparsedLabyrinthMatrix[i][ColumnIndex] == 0 || (!HasFrontNeighbor(i, ColumnIndex, TravellingDirection) && Neighbor == nullptr))
			{
				YOffset += 500.f;
				continue;
			}
			
			if(Neighbor == nullptr)
			{
				uint8 WallType = WallType = (ColumnIndex == 0 || ColumnIndex == std::size(UnparsedLabyrinthMatrix[0]) - 1) ? ChosenOuterWallMaterial : ChosenInsideWallMaterial;
				SpawnWall(SpawnLocation, TravellingDirection, WallType);
			}
			else Neighbor->AddSplinePoint(SpawnLocation);
		
			YOffset += 500.f;
		}
}

AProceduralSplineWall* ALabyrinthParser::SpawnWall(FVector& Location, ETravellingDirection TravellingDirection, uint8 WallType)
{
	if(ProceduralWallClass == nullptr) return nullptr;
	
	AProceduralSplineWall* ProceduralSplineWallInstance = GetWorld()->SpawnActor<AProceduralSplineWall>(ProceduralWallClass);
	ProceduralSplineWallInstance->SetActorScale3D(FVector(1.f, 1.f, 5.f));
	ProceduralSplineWallInstance->EditWallLook(WallType);
	
	if(ProceduralSplineWallInstance == nullptr) return nullptr;
	
	ProceduralSplineWallInstance->ClearSplinePoints();
	
	ProceduralSplineWallInstance->AddSplinePoint(Location);

	if(TravellingDirection == ETravellingDirection::ETD_Horizontal)
		ProceduralSplineWallInstancesHorizontal.Add(ProceduralSplineWallInstance);
	else if (TravellingDirection == ETravellingDirection::ETD_Vertical)
		ProceduralSplineWallInstancesVertical.Add(ProceduralSplineWallInstance);
	else if (TravellingDirection == ETravellingDirection::ETD_Flat)
		ProceduralSplineWallInstancesFlats.Add(ProceduralSplineWallInstance);
	
	return ProceduralSplineWallInstance;
}

AProceduralSplineWall* ALabyrinthParser::GetPossibleNeighborhood(FVector& Location, ETravellingDirection TravellingDirection)
{
	for(auto& WallInstance : TravellingDirection == ETravellingDirection::ETD_Horizontal ? ProceduralSplineWallInstancesHorizontal : ProceduralSplineWallInstancesVertical)
	{
		if(WallInstance->GetBackwardNeighbor(Location, TravellingDirection))
				return WallInstance;
	}
	
	return nullptr;
}

bool ALabyrinthParser::HasFrontNeighbor(uint8 Row, uint8 Column, ETravellingDirection TravellingDirection)
{
	switch(TravellingDirection)
	{
	case ETravellingDirection::ETD_Vertical:
		if(Row == std::size(UnparsedLabyrinthMatrix) - 1) return false;
		return UnparsedLabyrinthMatrix[Row+1][Column] != 0;
	case ETravellingDirection::ETD_Horizontal:
		if(Column == (std::size(UnparsedLabyrinthMatrix[0]) - 1)) return false;
		return UnparsedLabyrinthMatrix[Row][Column+1] != 0;
	default: ;
	}

	return false;
}

void ALabyrinthParser::MergeWalls(TArray<AProceduralSplineWall*>& FirstWalls, TArray<AProceduralSplineWall*>& SecondWalls, bool bSelfMerge)
{
	for(auto& FirstWall : FirstWalls)
	{
		// Primo punto che collide col top o bottom di un vertical
		FVector StartingPointHorizontal = FirstWall->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector EndingPointHorizontal = FirstWall->GetSplineComponent()->GetLocationAtSplinePoint(FirstWall->GetSplineComponent()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		AProceduralSplineWall* StartingPointCollider = nullptr;
		AProceduralSplineWall* EndingPointCollider = nullptr;
		
		for(auto& SecondWall : SecondWalls)
		{
			if(SecondWall == FirstWall) continue;
			
			FVector StartingPointVertical =
				SecondWall->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
			
			FVector EndingPointVertical = SecondWall->GetSplineComponent()->GetLocationAtSplinePoint(
				SecondWall->GetSplineComponent()->GetNumberOfSplinePoints() - 1,
				ESplineCoordinateSpace::World);

			if(StartingPointVertical == StartingPointHorizontal || EndingPointVertical == StartingPointHorizontal) StartingPointCollider = SecondWall;
			if(StartingPointVertical == EndingPointHorizontal || EndingPointVertical == EndingPointHorizontal) EndingPointCollider = SecondWall;
		}

		uint32 TotalPoints = 0;
		if(StartingPointCollider != nullptr)
		{
			TotalPoints = StartingPointCollider->GetSplineComponent()->GetNumberOfSplinePoints();
			if(StartingPointCollider == EndingPointCollider)
			{
				//TotalPoints -= 1;
				//FirstWall->GetSplineComponent()->SetClosedLoop(true, true); // Plugin not set to support closed loops splines
			}
			float FirstPointHeight =  StartingPointCollider->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World).Y;
			float WallPointHeight = FirstWall->GetSplineComponent()->GetLocationAtSplinePoint(FirstWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World).Y;

			if(FirstPointHeight >= WallPointHeight)
			{
				for(uint8 i = 1; i < TotalPoints; i++)
				{
					FVector PointLocation = StartingPointCollider->GetSplineComponent()->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
					FirstWall->GetSplineComponent()->AddSplinePointAtIndex(PointLocation, 0, ESplineCoordinateSpace::World);
				}
			}
			else
			{
				for(uint8 i = TotalPoints - 1; i > 0; i--)
				{
					FVector PointLocation = StartingPointCollider->GetSplineComponent()->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::World);
					FirstWall->GetSplineComponent()->AddSplinePointAtIndex(
						PointLocation, 0, ESplineCoordinateSpace::World);
				}	
			}

			if(!bSelfMerge)
			{
				MergedWalls.AddUnique(FirstWall);
				SecondWalls.Remove(StartingPointCollider);
				StartingPointCollider->Destroy();
			}
		};

		if(StartingPointCollider == EndingPointCollider) continue;
		
		if(EndingPointCollider != nullptr)
		{
			TotalPoints = EndingPointCollider->GetSplineComponent()->GetNumberOfSplinePoints();
			
			float FirstPointHeight =  EndingPointCollider->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World).Y;
			float WallPointHeight = FirstWall->GetSplineComponent()->GetLocationAtSplinePoint(FirstWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World).Y;
			
			if(FirstPointHeight >= WallPointHeight)
			{
				for(uint8 i = 1; i < TotalPoints; i++)
				{
					FVector PointLocation = EndingPointCollider->GetSplineComponent()->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
					FirstWall->GetSplineComponent()->AddSplinePointAtIndex(
						PointLocation, FirstWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s : %f : %d"), *EndingPointCollider->GetName(), FirstPointHeight, TotalPoints)
				for(uint8 i = TotalPoints - 1; i > 0; i--)
				{
					FVector PointLocation = EndingPointCollider->GetSplineComponent()->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::World);
					FirstWall->GetSplineComponent()->AddSplinePointAtIndex(
						PointLocation, FirstWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
				}	
			}
			if(!bSelfMerge)
			{
				MergedWalls.AddUnique(FirstWall);
				SecondWalls.Remove(EndingPointCollider);
				EndingPointCollider->Destroy();
			}
		}
	}
}

void ALabyrinthParser::SpawnCrossIntersection(uint8 RowIndex, uint8 ColumnIndex, FVector& SpawnLocation, ETravellingDirection TravellingDirection)
{
	if(RowIndex > 0 && RowIndex < std::size(UnparsedLabyrinthMatrix)
	&& ColumnIndex > 0 && ColumnIndex < std::size(UnparsedLabyrinthMatrix[RowIndex]) &&
	UnparsedLabyrinthMatrix[RowIndex-1][ColumnIndex] && UnparsedLabyrinthMatrix[RowIndex+1][ColumnIndex] && UnparsedLabyrinthMatrix[RowIndex][ColumnIndex+1]
	&& UnparsedLabyrinthMatrix[RowIndex][ColumnIndex-1])
		SpawnWall(SpawnLocation, TravellingDirection, UnparsedLabyrinthMatrix[RowIndex][ColumnIndex]);
}

void ALabyrinthParser::SpawnFlatSurface(bool bFloor)
{
	FVector Start = ProceduralSplineWallInstancesHorizontal[0]->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	AProceduralSplineWall* FlatWall = SpawnWall(Start, ETravellingDirection::ETD_Flat, ChosenOuterWallMaterial);
	
	FVector End = ProceduralSplineWallInstancesHorizontal[0]->GetSplineComponent()->GetLocationAtSplinePoint(
		ProceduralSplineWallInstancesHorizontal[0]->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
	
	FVector VerticalEnd = ProceduralSplineWallInstancesVertical[0]->GetSplineComponent()->GetLocationAtSplinePoint(
		ProceduralSplineWallInstancesVertical[0]->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
	
	
	FlatWall->AddSplinePoint(End);
	float WallHeight = ProceduralSplineWallInstancesVertical[0]->GetActorScale3D().Z;
	FlatWall->AddActorWorldOffset(FVector(0.f, 0.f, bFloor ? 0.f : WallHeight * 100.f));
	FlatWall->AddActorWorldRotation(FRotator(0.f, 0.f, 90.f));
	FlatWall->SetActorScale3D(FVector(1.f, 1.f, (VerticalEnd.Y - Start.Y) * 0.01f));
	FlatWall->AddPillarAtEnd(false);
	FlatWall->AddPillarAtStart(false);
	FlatWall->AddWallTop(false);
	ProceduralSplineWallInstancesFlats.Add(FlatWall);
}

void ALabyrinthParser::BuildLabyrinthInternal()
{
		ChosenOuterWallMaterial = OuterWallMaterials[FMath::RandRange(0, OuterWallMaterials.Num() - 1)];
		ChosenInsideWallMaterial = InsideWallMaterials[FMath::RandRange(0, InsideWallMaterials.Num() - 1)];

#pragma region CREATE LABYRINTH MESHES
	float Offset = 0.f;

	// Add horizontal splines
	for(uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix); i++)
	{
		TravelHorizontal(i, std::size(UnparsedLabyrinthMatrix[0]), ETravellingDirection::ETD_Horizontal, 0.f, 0.f + Offset);
		Offset += 500.f;
	}

	Offset = 0.f;

	// Add vertical splines
	for(uint8 i = 0; i < std::size(UnparsedLabyrinthMatrix[0]); i++)
	{
		TravelVertical(i, std::size(UnparsedLabyrinthMatrix), ETravellingDirection::ETD_Vertical, 0.f + Offset, 0.f);
		Offset += 500.f;
	}

	// Add floor & ceiling
	SpawnFlatSurface(false);
	SpawnFlatSurface(true);
	
	// Remove overlapping pillars
	for(auto& HorizontalSplineWall : ProceduralSplineWallInstancesHorizontal)
	{
		FVector HorizontalStartingPointLocation = HorizontalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector HorizontalEndingPointLocation = HorizontalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(HorizontalSplineWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

		//DrawDebugSphere(GetWorld(), HorizontalStartingPointLocation, 25.f, 20, FColor::Red, true);
		//DrawDebugSphere(GetWorld(), HorizontalEndingPointLocation, 25.f, 20, FColor::Red, true);
		
		for(auto& VerticalSplineWall : ProceduralSplineWallInstancesVertical)
		{
			FVector VerticalStartingPointLocation = VerticalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
			FVector VerticalEndingPointLocation = VerticalSplineWall->GetSplineComponent()->GetLocationAtSplinePoint(VerticalSplineWall->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

			if (HorizontalStartingPointLocation == VerticalStartingPointLocation || HorizontalStartingPointLocation == VerticalEndingPointLocation)
			{
				HorizontalSplineWall->AddPillarAtStart(false);
				//DrawDebugSphere(GetWorld(), HorizontalStartingPointLocation, 25.f, 20, FColor::Red, true);
			}

			if(HorizontalEndingPointLocation == VerticalEndingPointLocation || HorizontalEndingPointLocation == VerticalStartingPointLocation)
			{
				HorizontalSplineWall->AddPillarAtEnd(false);
				//DrawDebugSphere(GetWorld(), HorizontalEndingPointLocation, 25.f, 20, FColor::Red, true);
			}
		}
	}
	
#pragma endregion CREATE LABYRINTH MESHES
	
	for(const auto& ProceduralSplineWallPair: ProceduralSplineWallInstancesHorizontal)
	{
		if(ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == 3)
			ProceduralSplineWallPair->EditWallLook(13);
		
		ProceduralSplineWallPair->UpdateSplineMesh();
	}

	for(const auto& ProceduralSplineWallPair: ProceduralSplineWallInstancesVertical)
	{
		if(ProceduralSplineWallPair->GetSplineComponent()->GetNumberOfSplinePoints() == 3)
			ProceduralSplineWallPair->EditWallLook(13);
		ProceduralSplineWallPair->UpdateSplineMesh();
	}
	
	for(const auto& ProceduralSplineWallPair: ProceduralSplineWallInstancesFlats)
		ProceduralSplineWallPair->UpdateSplineMesh();

	ASceneController* SceneController = Cast<ASceneController>(UGameplayStatics::GetActorOfClass(this, ASceneController::StaticClass()));
	SceneController->OnLabyrinthParsedCompleteDelegate.Broadcast();
}

