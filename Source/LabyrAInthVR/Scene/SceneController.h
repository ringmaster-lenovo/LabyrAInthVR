// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneController.generated.h"

UCLASS()
class LABYRAINTHVR_API ASceneController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASceneController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Scene")
	bool SetupLevel(UObject* LabyrinthDTO)
	{
		// TODO: setup the scene and construct the labyrinth
		OnSceneReady.Broadcast();
		return true;  // return false if anything goes wrong
	};

	DECLARE_MULTICAST_DELEGATE(FSceneReadyEvent);
	FSceneReadyEvent OnSceneReady;
};
