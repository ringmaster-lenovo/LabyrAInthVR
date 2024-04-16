// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicController.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Music_Log);

// Sets default values
AMusicController::AMusicController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMusicController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMusicController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}