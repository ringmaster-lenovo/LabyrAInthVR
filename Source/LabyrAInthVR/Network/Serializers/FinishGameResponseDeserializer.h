#pragma once
#include "LabyrAInthVR/Network/DTO/LeaderBoardDTO.h"
#include "LabyrAInthVR/Network/DTO/FinishGameResponseDTO.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_FinishGameResponseSerializer_Log, Display, All);

class FinishGameDeserializer
{
public:
	static bool DeSerializeFinishGameResponse(FString FinishGameResponseString, UFinishGameResponseDTO* FinishGameResponseDTO);
	
};
