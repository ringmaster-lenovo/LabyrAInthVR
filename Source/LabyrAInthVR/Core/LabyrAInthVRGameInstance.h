// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LabyrAInthVRGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Core_Log, Display, All);

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ULabyrAInthVRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	virtual void Shutdown() override;

	static void SaveGame(const FString& PlayerName, int32 Level, int32 Time = INFINITY);

	// static void SaveGame(FString PlayerName, int32 Level, int32 Time = INFINITY, int32 Deaths, int32 EnemiesKilled, int32 TrapsExploded, int32 PowerUpsCollected);
};
