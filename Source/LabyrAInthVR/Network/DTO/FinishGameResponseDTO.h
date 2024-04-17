#pragma once

#include "CoreMinimal.h"
#include "LeaderBoardDTO.h"
#include "FinishGameResponseDTO.generated.h"

UCLASS()
class LABYRAINTHVR_API UFinishGameResponseDTO : public UObject
{

	GENERATED_BODY()
	
public:
	UFinishGameResponseDTO(){}
	
	TArray<ULeaderBoardDTO> Leaderboard;
};
