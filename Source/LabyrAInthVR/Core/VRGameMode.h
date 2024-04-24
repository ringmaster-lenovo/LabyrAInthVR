﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LabyrAInthVR/Core/VRGameState.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "LabyrAInthVR/Network/NetworkController.h"
#include "LabyrAInthVR/Scene/SceneController.h"
#include "LabyrAInthVR/Widgets/WidgetController.h"
#include "LabyrAInthVR/Player/BasePlayerController.h"
#include "LabyrAInthVR/Player/VRMainCharacter.h"
#include "VRGameMode.generated.h"

class AMain3DCharacter;
class AMusicController;
/**
 * This is the heart of the game. It will control the game state and the game flow.
 * It controls the game mode to be used in a level. It also controls the game state.
 * It needs to communicate with all the other controllers of the game hence the NetworkController, the SceneController, the PlayerController, the WidgetController
 *  It is a singleton class, so it can be accessed from anywhere in the game, but only one instance of it can exist at a time, and it is not exposed to blueprints or to the editor.
*/
UCLASS()
class LABYRAINTHVR_API AVRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVRGameMode();

protected:
	virtual void BeginPlay() override;

private:
	bool bIsVRHMDConnected;
	
	UPROPERTY()
	ABasePlayerController* BasePlayerController;

	UPROPERTY()
	AVRMainCharacter* CharacterVR;
	
	UPROPERTY()
	AMain3DCharacter* Character3D;

	UPROPERTY(EditDefaultsOnly,  Category = "Game Mode")
	TSubclassOf<AVRMainCharacter> CharacterVRClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	TSubclassOf<AMain3DCharacter> Character3DClass;

	UPROPERTY()
	ANetworkController* NetworkController;

	UPROPERTY()
	AWidgetController* WidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	TSubclassOf<AWidgetController> BP_WidgetController;

	UPROPERTY()
	AVRGameState* VRGameState;

	UPROPERTY()
	ASceneController* SceneController;

	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	TSubclassOf<ASceneController> BP_SceneController;

	UPROPERTY()
	AMusicController* MusicController;

	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	TSubclassOf<AMusicController> BP_MusicController;

	UPROPERTY()
	ULabyrinthDTO* LabyrinthDTO;

	UFUNCTION()
	void StartLobby();

	UFUNCTION()
	void PrepareGame();

	UFUNCTION()
	void MockNetwork();
	
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void PauseGame();

	UFUNCTION(BlueprintCallable)
	void ResumeGame();
	
	UFUNCTION()
	void EndGame(bool bIsWin = false);

	UFUNCTION()
	void RestartGame();

	UFUNCTION()
	void RePrepareGame();

	UFUNCTION()
	bool IsVRHMDConnected();

	UFUNCTION()
	void CrashCloseGame();

public:
	UFUNCTION(BlueprintCallable)
	void NewGameButtonClicked();

	UFUNCTION(BlueprintCallable)
	bool IsInVR() { return bIsVRHMDConnected; }
};
