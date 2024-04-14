// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"


#include "Engine/World.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"

DEFINE_LOG_CATEGORY(LogVR);

// This is the main character class for the VR game mode. It handles the VR camera, the VR controllers, and the VR movement.
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World) { return; }
}

// Called every frame
void AMainCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}