#pragma once
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthRequestDTO.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_LabyrinthSerializer_Log, Display, All);

class LabyrinthSerializer
{
public:
	static bool DeSerializeLabyrinth(FString LabyrinthString, ULabyrinthDTO* LabyrinthDTO);

	static FString SerializeLabyrinth(ULabyrinthRequestDTO* LabyrinthRequestDTO);
	
};
