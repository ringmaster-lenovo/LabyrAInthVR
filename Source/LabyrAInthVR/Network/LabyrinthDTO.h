#pragma once

#include "CoreMinimal.h"
#include "LabyrinthDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API ULabyrinthDTO : public UObject
{
	GENERATED_BODY()

public:
	ULabyrinthDTO(){}

	uint8 Level;

	std::vector<std::vector<uint8>> LabyrinthStructure;
};
