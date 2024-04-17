// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "3DPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API A3DPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	// UFUNCTION(BlueprintCallable, Category = "Input")
	// void EnableInputs(const bool bEnable) { bIsInputsEnabled = bEnable; }

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivate))
	// bool bIsInputsEnabled = true;
};
