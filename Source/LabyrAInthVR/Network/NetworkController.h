﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	FString GetLabyrinthFromBE(ULabyrinthDTO* LabyrinthDTO)
	{
		// TODO: get the labyrinth from ChatGPT and fill the LabyrinthDTO
		OnLabyrinthReceived.Broadcast();
		return "";  // return false if anything goes wrong
	};
	
	DECLARE_MULTICAST_DELEGATE(FLabyrinthReceivedEvent);
	FLabyrinthReceivedEvent OnLabyrinthReceived;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
