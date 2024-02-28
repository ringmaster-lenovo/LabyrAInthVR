// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetController.generated.h"

UCLASS()
class LABYRAINTHVR_API AWidgetController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWidgetController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
