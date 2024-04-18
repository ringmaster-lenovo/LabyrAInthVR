#include "FinishGameResponseDeserializer.h"

#include "LeaderboardSerializer.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_FinishGameResponseSerializer_Log);


bool FinishGameDeserializer::DeSerializeFinishGameResponse(FString FinishGameResponseString, UFinishGameResponseDTO* FinishGameResponseDTO)
{
	UE_LOG(LabyrAInthVR_FinishGameResponseSerializer_Log, Display, TEXT("Deserializing FinishGameResponse"));
	check(IsInGameThread());

	if (FinishGameResponseDTO == nullptr)
	{
		return false;
	}

	TSharedPtr<FJsonObject> OutFinishGameresponse;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(FinishGameResponseString);
	if (!FJsonSerializer::Deserialize(JsonReader, OutFinishGameresponse))
	{
		return false;
	}

	// Get the labyrinthStructure field from the Json object
	const TArray<TSharedPtr<FJsonValue>>* LeaderboardArray;
	if (!OutFinishGameresponse->TryGetArrayField(TEXT("leaderboard"), LeaderboardArray))
	{
		UE_LOG(LabyrAInthVR_FinishGameResponseSerializer_Log, Error, TEXT("Error during Leaderboard Deserialization: 'leaderboard' field not found or not an array."));
		return false;
	}
	// for each leaderboard in LeaderboardArray deserialize it and add it to the FinishGameResponseDTO
	for (auto Leaderboard : *LeaderboardArray)
	{
		ULeaderBoardDTO LeaderBoardDTO;
		if (LeaderboardSerializer::DeSerializeLeaderboard(Leaderboard->AsString(), &LeaderBoardDTO))
		{
			// TODO: decommentare che al momento non builda, capire perchè
			// TArray<ULeaderBoardDTO>* LeaderboardArray = &FinishGameResponseDTO->Leaderboard;
			// LeaderboardArray->Add(LeaderBoardDTO);
		}
		else
		{
			UE_LOG(LabyrAInthVR_FinishGameResponseSerializer_Log, Error, TEXT("Error during Leaderboard entry Deserialization: 'leaderboard' field not found or not an array."));
		}
	}
	UE_LOG(LabyrAInthVR_FinishGameResponseSerializer_Log, Display, TEXT("FinishGameResponse deserialized correctly"));
	return true;
}