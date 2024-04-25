// Fill out your copyright notice in the Description page of Project Settings.


#include "LabyrAInthVRGameInstance.h"

void ULabyrAInthVRGameInstance::Init()
{
	Super::Init();

	// Initialize any other game-specific components here
}

void ULabyrAInthVRGameInstance::Shutdown()
{
	Super::Shutdown();
}

void ULabyrAInthVRGameInstance::SaveGame(const FString& PlayerName, const int32 Level, const int32 Time)
{
	// Save the game state to GameSaves.csv in this format:
	// PlayerName, Level0, Time0, Level1, Time1, Level2, Time2, ...
	// If the file does not exist, create it
	// If the file exists, append to it
	// If the file exists and the PlayerName is already in it, search through the Levels and Times and overwrite the Time if the new Time is less than the old Time for the same Level

	// Open the file
	FString GameSavesPath = FPaths::ProjectSavedDir() + TEXT("SaveGames/GameSaves.csv");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(*GameSavesPath))
	{
		// Create the file
		FFileHelper::SaveStringToFile(TEXT(""), *GameSavesPath);
	}
	// Search for the PlayerName in the file, if it exists, overwrite its data, if not, append it
	FString FileData;
	FFileHelper::LoadFileToString(FileData, *GameSavesPath);
	TArray<FString> Lines;
	FileData.ParseIntoArrayLines(Lines);
	bool bPlayerNameFound = false;
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		FString Line = Lines[i];
		UE_LOG(LabyrAInthVR_Core_Log, Display, TEXT("Line: %s"), *Line);
		TArray<FString> Tokens;
		Line.ParseIntoArray(Tokens, TEXT(", "));  // Split the line by ", " as per the csv format
		if (Tokens[0] == PlayerName)
		{
			bool bLevelFound = false;
			for (int TokenIndex = 1; TokenIndex < Tokens.Num(); TokenIndex += 2)
			{
				if (FCString::Atoi(*Tokens[TokenIndex]) == Level)
				{
					// Overwrite the Time only if the new Time is less than the old Time for the same Level
					int32 OldTime = FCString::Atoi(*Tokens[TokenIndex + 1]);
					if (Time < OldTime)
					{
						// overwrite only the Time
						Tokens[TokenIndex + 1] = FString::FromInt(Time);
					}
					bLevelFound = true;
					break;
				}
			}
			if (!bLevelFound)
			{
				// Append the data
				Tokens.Add(FString::FromInt(Level));
				Tokens.Add(FString::FromInt(Time));
			}
			Lines[i] = FString::Join(Tokens, TEXT(", "));
			bPlayerNameFound = true;
			break;
		}
	}
	if (!bPlayerNameFound)
	{
		// Append the data
		Lines.Add(PlayerName + TEXT(", ") + FString::FromInt(Level) + TEXT(", ") + FString::FromInt(Time));
	}

	FFileHelper::SaveStringArrayToFile(Lines, *GameSavesPath);
}
