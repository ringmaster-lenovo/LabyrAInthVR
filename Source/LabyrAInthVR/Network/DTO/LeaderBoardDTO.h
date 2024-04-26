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
	uint16 Time;
	uint8 Level;
};