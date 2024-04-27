#pragma once

#include "CoreMinimal.h"
#include "FinishGameRequestDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API UFinishGameRequestDTO : public UObject
{
	GENERATED_BODY()

public:
	UFinishGameRequestDTO(){}

	FString Username;
	int16 Time;
	int8 Level;
};
