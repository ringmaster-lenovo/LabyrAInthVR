// Fill out your copyright notice in the Description page of Project Settings.


#include "VRGameState.h"

void AVRGameState::StartLevelTimer()
{
}

void AVRGameState::SetStateOfTheGame(EGameState NewState)
{
	CurrentStateOfTheGame = NewState;
}

EGameState AVRGameState::GetCurrentStateOfTheGame() const
{
	return CurrentStateOfTheGame;
}

void AVRGameState::SetCurrentLevel(uint8 NewLevel)
{
	CurrentLevel = NewLevel;
}

uint8 AVRGameState::GetCurrentLevel() const
{
	return CurrentLevel;
}

FString AVRGameState::GetCurrentStateOfTheGameAsString() const
{
	const TEnumAsByte GameStateEnum = CurrentStateOfTheGame;
	return UEnum::GetValueAsString(GameStateEnum.GetValue());
}
