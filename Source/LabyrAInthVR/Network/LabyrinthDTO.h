#pragma once

#include "CoreMinimal.h"
#include "LabyrinthDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API ULabyrinthDTO : public UObject
{
	GENERATED_BODY()

public:
	ULabyrinthDTO();

	uint8 Level = 0;

	TArray<TArray<uint8>> LabyrinthStructure;
};
