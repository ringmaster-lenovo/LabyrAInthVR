#include "LabyrinthSerializer.h"

#include "LabyrAInthVR/Network/DTO/LabyrinthRequestDTO.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_LabyrinthSerializer_Log);


bool LabyrinthSerializer::DeSerializeLabyrinth(FString LabyrinthString, ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Display, TEXT("Deserializing labyrinth"));
	check(IsInGameThread());

	if (LabyrinthDTO == nullptr)
	{
		return false;
	}

	TSharedPtr<FJsonObject> OutLabyrinth;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(LabyrinthString);
	if (!FJsonSerializer::Deserialize(JsonReader, OutLabyrinth))
	{
		return false;
	}
	
	// Get the level field from the Json object
	if (!OutLabyrinth->TryGetNumberField(TEXT("level"), LabyrinthDTO->Level))
	{
		UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Error, TEXT("Error during Level Deserialization: 'level' field not found or not a string."));
		return false;
	}

	// Get the width and height field from the Json object
	if (!OutLabyrinth->TryGetNumberField(TEXT("width"), LabyrinthDTO->Width))
	{
		UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Error, TEXT("Error during Width Deserialization: 'width' field not found or not a string."));
		return false;
	}

	if (!OutLabyrinth->TryGetNumberField(TEXT("height"), LabyrinthDTO->Height))
	{
		UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Error, TEXT("Error during Height Deserialization: 'height' field not found or not a string."));
		return false;
	}

	// Get the complexity field from the Json object
	if (!OutLabyrinth->TryGetNumberField(TEXT("complexity"), LabyrinthDTO->Complexity))
	{
		UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Error, TEXT("Error during Complexity Deserialization: 'complexity' field not found or not a string."));
		return false;
	}

	// Get the labyrinthStructure field from the Json object
	const TArray<TSharedPtr<FJsonValue>>* LabyrinthStructureArray;
	if (!OutLabyrinth->TryGetArrayField(TEXT("labyrinthStructure"), LabyrinthStructureArray))
	{
		UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Error, TEXT("Error during Labyrinth Structure Deserialization: 'labyrinthStructure' field not found or not an array."));
		return false;
	}
	
	// Extract the values from the Json array and store them in the LabyrinthDTO
	int32 LabyrinthRows = LabyrinthStructureArray->Num();
	int32 LabyrinthColumns = (LabyrinthRows > 0) ? (*LabyrinthStructureArray)[0]->AsArray().Num() : 0;
	UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Display, TEXT("Labyrinth Rows: %d, Labyrinth Columns: %d"), LabyrinthRows, LabyrinthColumns);
	LabyrinthDTO->LabyrinthStructure.resize(LabyrinthRows);
	for (auto& row : LabyrinthDTO->LabyrinthStructure) {
		row.resize(LabyrinthColumns, 0);
	}
	
	uint8 i = 0;
	for (auto LabyrinthRow : *LabyrinthStructureArray)
	{
		uint8 j = 0;
		for (auto Value : LabyrinthRow->AsArray())
		{
			uint8 LabValue;
			if (Value.IsValid() && Value->Type == EJson::Number && Value->TryGetNumber(LabValue))
			{
				LabyrinthDTO->LabyrinthStructure[i][j] = LabValue;
			}
			j++;
		}
		i++;
	}
	UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Display, TEXT("Labyrinth deserialized correctly"));
	return true;
}

FString LabyrinthSerializer::SerializeLabyrinth(ULabyrinthRequestDTO* LabyrinthRequestDTO)
{
	UE_LOG(LabyrAInthVR_LabyrinthSerializer_Log, Log, TEXT("Serializing Request for labyrinth"));
	// Create a Json object to hold the LabyrinthRequestDTO
	TSharedPtr<FJsonObject> LabyrinthRequestDTOJson = MakeShareable(new FJsonObject);
	
	// Set the level field
	LabyrinthRequestDTOJson->SetNumberField(TEXT("level"), LabyrinthRequestDTO->Level);
	
	// Serialize the Json object to a string
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(LabyrinthRequestDTOJson.ToSharedRef(), JsonWriter);
	return JsonString;
}