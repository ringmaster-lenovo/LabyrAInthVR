#pragma once

#include "CoreMinimal.h"
#include "FinishGameRequestDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API UFinishGameRequestDTO : public UObject
{
	GENERATED_BODY()

public:
	UFinishGameRequestDTO(){}

	FString username;
	
	int16 score;
};
