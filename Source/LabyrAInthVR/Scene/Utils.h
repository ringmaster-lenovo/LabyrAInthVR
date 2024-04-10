// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Utils.generated.h"

/**
 * Struct containing the information about the actors spawned:
 * NumOfActorSpawned: the number of actors spawned
 * ActorsLocations: a set of the locations (indexes to be translated to row and col) of the actors spawned
 */
USTRUCT()
struct FInfoActorSpawned
{
	GENERATED_BODY()

	UPROPERTY()
	int NumOfActorSpawned = 0;

	UPROPERTY()
	TArray<int> ActorsLocations = {};
};

/**
 * Utility class
 * Contains utility functions: ConvertToIndex, ConvertToRowColumn, MatrixToString, StructToString
 */
UCLASS()
class LABYRAINTHVR_API UUtils : public UObject
{
	GENERATED_BODY()

public:
	static int ConvertToIndex(int Row, int Column);

	static void ConvertToRowColumn(int Index, int& Row, int& Column);

	static FString MatrixToString(const std::vector<std::vector<uint8>>* Matrix);

	static FString StructToString(const FInfoActorSpawned& InfoActorSpawned);

	static FInfoActorSpawned GetInfoActorSpawned(const int NumOfActorSpawned, const TArray<int>* ActorsSpawnedLocations);
};
