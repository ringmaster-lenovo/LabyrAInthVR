﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VRGameState.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Egs_InMainMenu           = 0 UMETA(DisplayName = "In Main Menu"),
	Egs_WaitingForLabyrinth  = 1 UMETA(DisplayName = "Waiting For Labyrinth"),
	Egs_Playing              = 2 UMETA(DisplayName = "Playing"),
	Egs_Pausing              = 3 UMETA(DisplayName = "Pausing"),
	Egs_Restarting           = 4 UMETA(DisplayName = "Restarting"),
	Egs_Closing              = 5 UMETA(DisplayName = "Closing")
};

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API AVRGameState : public AGameStateBase
{
	GENERATED_BODY()

private:
	EGameState CurrentStateOfTheGame;
	
	uint8 CurrentLevel = 0;
	
	uint32 SecondsSinceStartOfLevel = 0;

	UFUNCTION(Category = "Game State")
	void StartLevelTimer();
	
public:
	UFUNCTION(Category = "Game State")
	void SetStateOfTheGame(EGameState NewState);
	
	UFUNCTION(Category = "Game State")
	EGameState GetCurrentStateOfTheGame();

	UFUNCTION(Category = "Game State")
	void SetCurrentLevel(uint8 NewLevel);

	UFUNCTION(Category = "Game State")
	uint8 GetCurrentLevel();
};