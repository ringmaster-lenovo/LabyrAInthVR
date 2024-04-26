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

void ULabyrAInthVRGameInstance::SaveGame(const FString& PlayerName, const int Level, const int Time)
{
	// Save the game state to GameSaves.csv in this format:
	// PlayerName, Level0, Time0, Level1, Time1, Level2, Time2, ...
	// If the file does not exist, create it
	// If the file exists, append to it
	// If the file exists and the PlayerName is already in it, search through the Levels and Times and overwrite the Time if the new Time is less than the old Time for the same Level

	// Open the file
	const FString GameSavesPath = FPaths::ProjectSavedDir() + TEXT("SaveGames/GameSaves.csv");
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

void ULabyrAInthVRGameInstance::SaveGameStats(const FString& PlayerName, const int Level, const int Rows, const int Columns,
	const int Complexity, const int Time, const int Deaths, const int EnemiesKilled, const int TrapsExploded, const int PowerUpsCollected, const int WeaponsFound)
{
	// Save the game statistics to GameSaves.csv in this format:
	// PlayerName, Level0, Time0, Level1, Time1, Level2, Time2, ...
	// If the file does not exist, create it
	// If the file exists, append to it
	// If the file exists and the PlayerName is already in it, search through the Levels and Times and overwrite the Time if the new Time is less than the old Time for the same Level

	// Open the file
	const FString GameSavesPath = FPaths::ProjectSavedDir() + TEXT("SaveGames/GameSavesStats.csv");
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
			for (int TokenIndex = 1; TokenIndex < Tokens.Num(); TokenIndex += 10)
			{
				if (FCString::Atoi(*Tokens[TokenIndex]) == Level)
				{
					// Overwrite the Time only if the new Time is less than the old Time for the same Level
					int32 OldTime = FCString::Atoi(*Tokens[TokenIndex + 1]);
					if (Time < OldTime)
					{
						// overwrite Rows, Columns, Complexity and Time
						Tokens[TokenIndex + 1] = FString::FromInt(Rows);
						Tokens[TokenIndex + 2] = FString::FromInt(Columns);
						Tokens[TokenIndex + 3] = FString::FromInt(Complexity);
						Tokens[TokenIndex + 4] = FString::FromInt(Time);
					}
					// then add the rest of the stats
					Tokens[TokenIndex + 5] += FString::FromInt(Deaths);
					Tokens[TokenIndex + 6] += FString::FromInt(EnemiesKilled);
					Tokens[TokenIndex + 7] += FString::FromInt(TrapsExploded);
					Tokens[TokenIndex + 8] += FString::FromInt(PowerUpsCollected);
					Tokens[TokenIndex + 9] += FString::FromInt(WeaponsFound);
					bLevelFound = true;
					break;
				}
			}
			if (!bLevelFound)
			{
				// Append the data
				Tokens.Add(FString::FromInt(Level));
				Tokens.Add(FString::FromInt(Rows));
				Tokens.Add(FString::FromInt(Columns));
				Tokens.Add(FString::FromInt(Complexity));
				Tokens.Add(FString::FromInt(Time));
				Tokens.Add(FString::FromInt(Deaths));
				Tokens.Add(FString::FromInt(EnemiesKilled));
				Tokens.Add(FString::FromInt(TrapsExploded));
				Tokens.Add(FString::FromInt(PowerUpsCollected));
				Tokens.Add(FString::FromInt(WeaponsFound));
			}
			Lines[i] = FString::Join(Tokens, TEXT(", "));
			bPlayerNameFound = true;
			break;
		}
	}
	if (!bPlayerNameFound)
	{
		// Append the data
		Lines.Add(PlayerName + TEXT(", ") + FString::FromInt(Level) + TEXT(", ") + FString::FromInt(Rows) + TEXT(", ") + FString::FromInt(Columns) + TEXT(", ") + FString::FromInt(Complexity)+ TEXT(", ") + FString::FromInt(Time) + TEXT(", ")
			+ FString::FromInt(Deaths) + TEXT(", ") + FString::FromInt(EnemiesKilled) + TEXT(", ") + FString::FromInt(TrapsExploded) + TEXT(", ") + FString::FromInt(PowerUpsCollected) + TEXT(", ") + FString::FromInt(WeaponsFound));
	}

	FFileHelper::SaveStringArrayToFile(Lines, *GameSavesPath);
}
