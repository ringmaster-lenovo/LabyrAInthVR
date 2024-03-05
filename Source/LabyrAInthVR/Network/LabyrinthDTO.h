// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LabyrinthDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API ULabyrinthDTO: public UObject
{
	GENERATED_BODY()
public:
	ULabyrinthDTO()
	{};
    
	uint8 Level = 0;
	TArray<TArray<int>> LabyrinthStructure;
};