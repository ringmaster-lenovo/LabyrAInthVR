// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Player_Log, Display, All);

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void EnableInputs(const bool bEnable) { bIsInputsEnabled = bEnable; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate))
	bool bIsInputsEnabled = true;

	UFUNCTION(Category = "GameLogic")
	FString TeleportPlayer(const FVector& Position, const FRotator& Rotation) const;

	UFUNCTION(BlueprintCallable, Category = "GameLogic")
	void CollidedWithEndPortal();

	DECLARE_MULTICAST_DELEGATE(FOnCollisionWithEndPortal);
	FOnCollisionWithEndPortal OnCollisionWithEndPortal;
};
