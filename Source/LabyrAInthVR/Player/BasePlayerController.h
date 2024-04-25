﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "NetworkMessage.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class AMainCharacter;
DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Player_Log, Display, All);

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	AMainCharacter* MainCharacter;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void EnableInputs(const bool bEnable) { bIsInputsEnabled = bEnable; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate))
	bool bIsInputsEnabled = true;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetControlledCharacter(AMainCharacter* AMainCharacter);

	UFUNCTION(BlueprintCallable, Category = "Player")
	AMainCharacter* GetControlledCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetPlayerName() const;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetPlayerName(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Player")
	int32 GetPlayerTimeOnCurrentLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void ResetPlayerStats();

	UFUNCTION(Category = "GameLogic")
	FString TeleportPlayer(const FVector& Position, const FRotator& Rotation, const bool InGame = true);

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void CollidedWithEndPortal() const;

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void PlayerHasDied() const;

	bool InGame;

	DECLARE_MULTICAST_DELEGATE(FOnCollisionWithEndPortal);
	FOnCollisionWithEndPortal OnCollisionWithEndPortal;

	DECLARE_MULTICAST_DELEGATE(FOnPlayerHasDied);
	FOnPlayerHasDied OnPLayerDeath;
};
