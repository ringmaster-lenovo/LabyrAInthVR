#pragma once

#include "CoreMinimal.h"
#include "LeaderBoardDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API ULeaderBoardDTO : public UObject
{
	GENERATED_BODY()

public:
	ULeaderBoardDTO(){}

	FString Username;
	int16 Score;
	int8 LabyrinthComplexity;
};
