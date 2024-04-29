// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interfaces/MovableActor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "LabyrAInthVR/Player/PlayerStatistics.h"
#include "PowerUp.generated.h"

class UStatsChangerComponent;

UCLASS()
class LABYRAINTHVR_API APowerUp : public AActor, public ISpawnableActor, public IMovableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerUp();

	/** Sound to play on collect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USoundBase* CollectSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
