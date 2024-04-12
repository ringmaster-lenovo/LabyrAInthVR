#pragma once
#include "DTO/LabyrinthDTO.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_LabyrinthSerializer_Log, Display, All);

class LabyrinthSerializer
{
public:
	static bool DeSerializeLabyrinth(FString LabyrinthString, ULabyrinthDTO* LabyrinthDTO);

	static FString SerializeLabyrinth(ULabyrinthDTO* LabyrinthDTO);
	
};
