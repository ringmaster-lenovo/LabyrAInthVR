#include "LeaderboardSerializer.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_LeaderboardSerializer_Log);


bool LeaderboardSerializer::DeSerializeLeaderboard(FString LeaderboardString, ULeaderBoardDTO* LeaderboardDTO)
{
	UE_LOG(LabyrAInthVR_LeaderboardSerializer_Log, Display, TEXT("Deserializing labyrinth"));
	check(IsInGameThread());

	if (LeaderboardDTO == nullptr)
	{
		return false;
	}

	TSharedPtr<FJsonObject> OutLeaderboard;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(LeaderboardString);
	if (!FJsonSerializer::Deserialize(JsonReader, OutLeaderboard))
	{
		return false;
	}
	
	// Get the username field from the Json object
	if (!OutLeaderboard->TryGetStringField(TEXT("username"), LeaderboardDTO->Username))
	{
		UE_LOG(LabyrAInthVR_LeaderboardSerializer_Log, Error, TEXT("Error during Username Deserialization: 'username' field not found or not a string."));
		return false;
	}

	// Get the score field from the Json object
	if (!OutLeaderboard->TryGetNumberField(TEXT("score"), LeaderboardDTO->Score))
	{
		UE_LOG(LabyrAInthVR_LeaderboardSerializer_Log, Error, TEXT("Error during Score Deserialization: 'score' field not found or not an array."));
		return false;
	}

	// Get the labyrinthComplexity field from the Json object
	if (!OutLeaderboard->TryGetNumberField(TEXT("labyrinthComplexity"), LeaderboardDTO->LabyrinthComplexity))
	{
		UE_LOG(LabyrAInthVR_LeaderboardSerializer_Log, Error, TEXT("Error during labyrinthComplexity Deserialization: 'labyrinthComplexity' field not found or not an array."));
		return false;
	}
	UE_LOG(LabyrAInthVR_LeaderboardSerializer_Log, Display, TEXT("Leaderboard deserialized correctly"));
	return true;
}

FString LeaderboardSerializer::SerializeLeaderboard(ULeaderBoardDTO* LeaderboardDTO)
{
	UE_LOG(LabyrAInthVR_LeaderboardSerializer_Log, Log, TEXT("Serializing Request for leaderboard"));
	// Create a Json object to hold the LeaderboardDTO
	TSharedPtr<FJsonObject> LeaderboardDTOJson = MakeShareable(new FJsonObject);
	
	// Set the leaderboard fields
	LeaderboardDTOJson->SetStringField(TEXT("username"), LeaderboardDTO->Username);
	LeaderboardDTOJson->SetNumberField(TEXT("score"), LeaderboardDTO->Score);
	LeaderboardDTOJson->SetNumberField(TEXT("labyrinthComplexity"), LeaderboardDTO->LabyrinthComplexity);
	
	// Serialize the Json object to a string
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(LeaderboardDTOJson.ToSharedRef(), JsonWriter);
	return JsonString;
}