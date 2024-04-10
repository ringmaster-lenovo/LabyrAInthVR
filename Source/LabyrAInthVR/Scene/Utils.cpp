// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils.h"
#include "Config.h"

/**
 * Convert the row and column to an index
 * @param Row the row
 * @param Column the column
 * @return int index = row * LabyrinthWidth + column
 */
int UUtils::ConvertToIndex(const int Row, const int Column)
{
	return Row * WallSettings::LabyrinthWidth + Column;	
}

/**
 * Convert the index to a row and a column
 * @param Index the index
 * @param Row the row
 * @param Column the column
 */
void UUtils::ConvertToRowColumn(const int Index, int& Row, int& Column)
{
	Row = Index / WallSettings::LabyrinthWidth;
	Column = Index % WallSettings::LabyrinthWidth;
}

/**
 * Convert the labyrinth matrix to a string for debugging purposes
 * @param Matrix the labyrinth matrix
 * @return FString the labyrinth matrix as a string
 */
FString UUtils::MatrixToString(const std::vector<std::vector<uint8>>* Matrix)
{
	// // convert the labyrinth matrix to a string for debugging purposes
	FString MatrixString = "";
	for (const std::vector<uint8>& Row : *Matrix)
	{
		for (const uint8& Column : Row)
		{
			MatrixString += FString::FromInt(Column) + " ";
		}
		MatrixString += "\n";
	}
	return MatrixString;
}

/**
 * Convert the struct containing the information about the actors spawned to a string for debugging purposes
 * @param InfoActorSpawned the struct containing the information about the actors spawned
 * @return FString the information about the actors spawned as a string
 */
FString UUtils::StructToString(const FInfoActorSpawned& InfoActorSpawned)
{
	FString InfoString = "Number of actors spawned: " + FString::FromInt(InfoActorSpawned.NumOfActorSpawned) + "\n";
	InfoString += "Locations:\n";
	for (const int& Location : InfoActorSpawned.ActorsLocations)
	{
		int Row = -1;
		int Column = -1;
		ConvertToRowColumn(Location, Row, Column);
		InfoString += "row: " + FString::FromInt(Row) + ", column: " + FString::FromInt(Column) + "\n";
	}
	return InfoString;
}

/**
 * Get a structure wth the info of the actors spawned passed by
 * @param NumOfActorSpawned the number of actors spawned
 * @param ActorsSpawnedLocations the locations of the actors spawned
 * @return FInfoActorSpawned the struct containing the number of actors spawned and their locations
 */
FInfoActorSpawned UUtils::GetInfoActorSpawned(const int NumOfActorSpawned, const TArray<int>* ActorsSpawnedLocations)
{
	FInfoActorSpawned Info;
	Info.NumOfActorSpawned = NumOfActorSpawned;
	Info.ActorsLocations = *ActorsSpawnedLocations;
	return Info;
}