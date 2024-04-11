#pragma once

#include "CoreMinimal.h"

UCLASS()
class LABYRAINTHVR_API ULeaderBoardDTO : public UObject
{
	GENERATED_BODY()

public:
	ULeaderBoardDTO(){}

	FString username;
	
	int16 score;
};
