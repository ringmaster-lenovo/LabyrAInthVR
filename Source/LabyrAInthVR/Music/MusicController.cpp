// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicController.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Music_Log);

// Sets default values
AMusicController::AMusicController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false; // Prevent it from playing immediately
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

void AMusicController::PlayClockSound(int32 seconds)
{
	if (ClockSound && AudioComponent)
	{
		AudioComponent->SetVolumeMultiplier(CurrentVolumeMultiplier);
		if (!AudioComponent->IsPlaying())
		{
			AudioComponent->SetSound(ClockSound);
			AudioComponent->Play();
		}
		if (seconds > 30)
		{
			CurrentVolumeMultiplier += 0.01f;
		}
		else if (seconds > 10)
		{
			CurrentVolumeMultiplier += 0.02f;
		}
		else {
			CurrentVolumeMultiplier += 0.05f;
		}
		// CurrentVolumeMultiplier += 0.025f;
	    UE_LOG(LabyrAInthVR_Music_Log, Display, TEXT("Volume increased: %f"), CurrentVolumeMultiplier);
	}
}

void AMusicController::StopClockSound()
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
	}
}

void AMusicController::ResetVolumeMultiplier()
{
	CurrentVolumeMultiplier = DefaultCurrentVolumeMultiplier;
}
