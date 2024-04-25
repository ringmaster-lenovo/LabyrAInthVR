// Fill out your copyright notice in the Description page of Project Settings.


#include "VRGameState.h"

#include "LabyrAInthVRGameInstance.h"


void AVRGameState::StartPlayerTimer()
{
	//START CHRONOMETER
	if (!GetWorld()) return; // Ensure we have a valid world context before starting the timer
	GetWorld()->GetTimerManager().SetTimer(PlayerGameTimeHandle, this, &AVRGameState::UpdatePlayerGameTime, 1.0f, true);
}

void AVRGameState::UpdatePlayerGameTime()
{
	PlayerGameTimeInSec++;
}

void AVRGameState::StopPlayerTimer()
{
	//STOP CHRONOMETER
	if (!GetWorld()) return; // Ensure we have a valid world context before stopping the timer
	GetWorld()->GetTimerManager().ClearTimer(PlayerGameTimeHandle);
}

void AVRGameState::SetStateOfTheGame(const EGameState NewState)
{
	CurrentStateOfTheGame = NewState;
}

EGameState AVRGameState::GetCurrentStateOfTheGame() const
{
	return CurrentStateOfTheGame;
}

FString AVRGameState::GetCurrentStateOfTheGameAsString() const
{
	const TEnumAsByte GameStateEnum = CurrentStateOfTheGame;
	return UEnum::GetValueAsString(GameStateEnum.GetValue());
}

void AVRGameState::SetCurrentLevel(const uint8 NewLevel)
{
	CurrentLevel = NewLevel;
}

uint8 AVRGameState::GetCurrentLevel() const
{
	return CurrentLevel;
}

FString AVRGameState::LoginPlayer(FString NewPlayerName)
{
	if (NewPlayerName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player name is empty!"));
		return "Player name is empty!";
	}
	if (NewPlayerName.Len() > 50)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player name is too long!"));
		return "Player name is too long!";
	}
	if (NewPlayerName == DefaultPlayerName)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player name is default!"));
		return "Player name is default!";
	}
	// define a regular expression pattern to match only alphanumeric characters
	FRegexPattern Pattern(TEXT("^[a-zA-Z0-9]*$"));
	FRegexMatcher Matcher(Pattern, NewPlayerName);
	if (!Matcher.FindNext())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player name contains invalid characters!"));
		return "Player name contains invalid characters!";
	}
	// transform the player name to uppercase to have font consistency
	NewPlayerName = NewPlayerName.ToUpper();
	UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Player logged in, name set to %s"), *NewPlayerName);
	PlayerName = NewPlayerName;
	bIsLoggedIn = true;
	StartPlayerTimer();  // TODO: search the game time form the gave saves file
	return "";
}

FString AVRGameState::GetPlayerName() const
{
	if (!bIsLoggedIn)
	{
		return DefaultPlayerName;
	}
	return PlayerName;
}
 
void AVRGameState::LogOutPlayer()
{
	PlayerName = DefaultPlayerName;
	bIsLoggedIn = false;
	UE_LOG(LabyrAInthVR_Core_Log, Warning, TEXT("Player logged out"));
}

bool AVRGameState::IsLoggedIn() const
{
	return bIsLoggedIn;
}

void AVRGameState::SetPlayerGameTime(const uint32 NewTime)
{
	PlayerGameTimeInSec = NewTime;
}

uint32 AVRGameState::GetPlayerGameTime() const
{
	return PlayerGameTimeInSec;
}
