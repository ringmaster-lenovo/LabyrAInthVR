#pragma once

#include "CoreMinimal.h"
#include "LabyrinthRequestDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API ULabyrinthRequestDTO : public UObject
{
	GENERATED_BODY()

public:
	ULabyrinthRequestDTO(){}

	uint8 Level;

	uint8 GetDefaultLevel()
	{
		return 3;
	}

};
