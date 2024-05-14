// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DTO/FinishGameRequestDTO.h"
#include "DTO/FinishGameResponseDTO.h"
#include "DTO/LabyrinthRequestDTO.h"
#include "GameFramework/GameNetworkManager.h"
#include "Interfaces/IHttpRequest.h"
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
	void GetLabyrinthFromBE(ULabyrinthRequestDTO* LabyrinthRequestDTO, ULabyrinthDTO* LabyrinthDTO);

	UFUNCTION(Category = "Network")
	void FinishGame(UFinishGameRequestDTO* FinishGameRequestDTO, UFinishGameResponseDTO* FinishGameResponseDTO);

	UFUNCTION(Category = "Network")
	void GetAllLeaderboards(ULeaderBoardDTO* LeaderBoardDTO);
	
	DECLARE_MULTICAST_DELEGATE(FLabyrinthReceivedEvent);
	FLabyrinthReceivedEvent OnLabyrinthReceived;

	DECLARE_MULTICAST_DELEGATE(FFinishGameResponseReceivedEvent);
	FFinishGameResponseReceivedEvent OnFinishGameResponseReceived;

	DECLARE_MULTICAST_DELEGATE(FNetworkErrorEvent);
	FNetworkErrorEvent OnNetworkError;

	DECLARE_MULTICAST_DELEGATE(FFinishGameErrorEvent);
	FFinishGameErrorEvent OnFinishGameError;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	FString BaseURL = "https://localhost:8080/api";
	FString LabyrinthEndPoint = "/labyrinth/get-labyrinth-from-dataset";
	FString LeaderboardEndPoint = "/leaderboards";
	FString LabyrinthURL = "http://localhost:8090/gpm/api/labyrinth/get-labyrinth-from-dataset";;
	FString LeaderboardUrl = "https://localhost:8080/api/leaderboards";

	bool DeserializeAllLeaderBoards(FString AllLeaderBoardsString, ULeaderBoardDTO* LeaderBoardDTO);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetRequest(FString Url, FString Method, FString* ObjectDTO);
};
