#pragma once

#include "CoreMinimal.h"
#include "LeaderBoard.generated.h"

UCLASS()
class LABYRAINTHVR_API ULeaderBoardDTO : public UObject
{
	GENERATED_BODY()

public:
	ULeaderBoardDTO(){}

	FString username;
	
	int16 score;
};
