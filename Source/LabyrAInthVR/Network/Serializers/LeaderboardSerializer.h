#pragma once
#include "LabyrAInthVR/Network/DTO/LeaderBoardDTO.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_LeaderboardSerializer_Log, Display, All);

class LeaderboardSerializer
{
public:
	static bool DeSerializeLeaderboard(TSharedPtr<FJsonObject> LeaderboardObject, ULeaderBoardDTO* LeaderboardDTO);

	static FString SerializeLeaderboard(ULeaderBoardDTO* LeaderboardDTO);
	
};
