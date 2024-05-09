// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VRGameState.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Egs_NotYetStarted         = 0 UMETA(DisplayName = "Game not yet started"),
	Egs_InMainMenu            = 1 UMETA(DisplayName = "In Main Menu"),
	Egs_WaitingForLabyrinth   = 2 UMETA(DisplayName = "Waiting For Labyrinth"),
	Egs_WaitingForSceneBuild  = 3 UMETA(DisplayName = "Waiting For Scene Build"),
	Egs_Playing               = 4 UMETA(DisplayName = "Playing"),
	Egs_Pausing               = 5 UMETA(DisplayName = "Pausing"),
	Egs_Ending                = 6 UMETA(DisplayName = "Ending"),
	Egs_Restarting            = 7 UMETA(DisplayName = "Restarting"),
	Egs_RePreparing           = 8 UMETA(DisplayName = "RePreparing"),
	Egs_ClosingGame           = 9 UMETA(DisplayName = "ClosingGame")
};

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API AVRGameState : public AGameStateBase
{
	GENERATED_BODY()

private:
	EGameState CurrentStateOfTheGame = EGameState::Egs_NotYetStarted;

	FString DefaultPlayerName = "Player";

	FString PlayerName;

	bool bIsLoggedIn = false;
	
	uint8 CurrentLevel = 1;

	FTimerHandle PlayerGameTimeHandle;
	
	uint32 PlayerGameTimeInSec = 0;

	UFUNCTION(Category = "Game State")
	void StartPlayerTimer();

	void UpdatePlayerGameTime();
	
public:
	UFUNCTION(Category = "Game State")
	void SetStateOfTheGame(EGameState NewState);
	
	UFUNCTION(Category = "Game State")
	EGameState GetCurrentStateOfTheGame() const;

	UFUNCTION()
	FString GetCurrentStateOfTheGameAsString() const;
	
	UFUNCTION(Category = "Game State")
	void SetCurrentLevel(uint8 NewLevel);

	UFUNCTION(Category = "Game State")
	uint8 GetCurrentLevel() const;

	UFUNCTION(Category = "Player")
	FString LoginPlayer(FString NewPlayerName);

	UFUNCTION(Category = "Player")
	FString GetPlayerName() const;

	UFUNCTION(Category = "Player")
	void LogOutPlayer();

	UFUNCTION(Category = "Player")
	bool IsLoggedIn() const;

	UFUNCTION(Category = "Player")
	void SetPlayerGameTime(uint32 NewTime);

	UFUNCTION(Category = "Player")
	uint32 GetPlayerGameTime() const;
	
	UFUNCTION(Category = "Game State")
	void StopPlayerTimer();
};