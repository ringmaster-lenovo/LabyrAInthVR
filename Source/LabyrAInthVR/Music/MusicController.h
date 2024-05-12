// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "MusicController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Music_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API AMusicController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMusicController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void StartAmbienceMusic(bool bIsInLobby);

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void StartFinalResultMusic(bool bHasWon);

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void StartCombatMusic();

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void StartClockTick();

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void StopCombatMusic();

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void StopMusic();

	UFUNCTION(BlueprintImplementableEvent, Category=Music)
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void PlayClockSound(int32 seconds);

	UFUNCTION(BlueprintCallable)
	void StopClockSound();

	UFUNCTION(BlueprintCallable)
	void ResetVolumeMultiplier();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Music)
	bool bIsMusicPlaying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Music)
	bool bInLobby = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Music)
	float MusicVolume = 1.0f;
	
	float DefaultCurrentVolumeMultiplier = 0.0f;

	float CurrentVolumeMultiplier = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ClockSound;
};
