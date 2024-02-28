// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameNetworkManager.h"
#include "NetworkController.generated.h"

UCLASS()
class LABYRAINTHVR_API ANetworkController : public AGameNetworkManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetworkController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
