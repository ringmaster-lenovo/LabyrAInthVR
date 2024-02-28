// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LabyrAInthVRGameInstance.generated.h"

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
};
