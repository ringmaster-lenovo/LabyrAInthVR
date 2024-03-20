#pragma once

#include "CoreMinimal.h"
#include "LabyrinthDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API ULabyrinthDTO : public UObject
{
	GENERATED_BODY()

public:
	ULabyrinthDTO()
	{
		Level = 1;
		// LabyrinthStructure.resize(10, std::vector<uint8>(10, 0)); // Inizializza LabyrinthStructure con una griglia 5x5 di valori 0
	}

	uint8 Level = 1;

	std::vector<std::vector<uint8>> LabyrinthStructure;
};
