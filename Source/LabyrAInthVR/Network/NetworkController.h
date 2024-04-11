// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DTO/FinishGameRequestDTO.h"
#include "DTO/LeaderBoard.h"
#include "GameFramework/GameNetworkManager.h"
#include "NetworkController.generated.h"

class ULabyrinthDTO;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Network_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API ANetworkController : public AGameNetworkManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetworkController();

	UFUNCTION(Category = "Network")
	void GetLabyrinthFromBE(ULabyrinthDTO* LabyrinthDTO);

	UFUNCTION(Category = "Network")
	void FinishGame(UFinishGameRequestDTO FinishGameRequestDTO);

	UFUNCTION(Category = "Network")
	TArray<ULeaderBoardDTO> GetAllLeaderboards();
	
	DECLARE_MULTICAST_DELEGATE(FLabyrinthReceivedEvent);
	FLabyrinthReceivedEvent OnLabyrinthReceived;

	DECLARE_MULTICAST_DELEGATE(FNetworkErrorEvent);
	FNetworkErrorEvent OnNetworkError;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	FString BaseURL = "https://localhost:8080";
	FString LabyrinthURL = BaseURL + "/labyrinth";
	FString LeaderboardUrl = BaseURL + "/leaderboards";

	FString SerializeLabyrinth(ULabyrinthDTO* LabyrinthDTO);

	bool DeSerializeLabyrinth(FString LabyrinthString, ULabyrinthDTO* LabyrinthDTO);
	bool DeserializeAllLeaderBoards(FString AllLeaderBoardsString, ULeaderBoardDTO* LeaderBoardDTO);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetRequest(FString Url, FString Method, FString* ObjectDTO);

};
