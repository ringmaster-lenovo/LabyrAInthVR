// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"
#include "Components/AudioComponent.h"

class USoundCue;

// UStartMenuWidget::UStartMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
// {
// 	BackgroundMusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundMusicAudioComponent"));
// 	BackgroundMusicAudioComponent->bAutoActivate = false; // Do not play the sound immediately.
//
// 	// Load our Sound Cue for the propeller sound we created in the editor... note your path may be different depending
// 	// on where you store the asset on disk.
// 	static ConstructorHelpers::FObjectFinder<USoundBase>soundCue(TEXT("/Script/Engine.SoundCue'/Game/StarterContent/Audio/Starter_Music_Cue.Starter_Music_Cue'"));
//
// 	// Store a reference to the Cue asset - we'll need it later.
// 	BackgroudMusicCue = soundCue.Object;
// }

void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddUniqueDynamic(this, &UStartMenuWidget::OnStartClicked);
	}
}

void UStartMenuWidget::OnStartClicked()
{
	if (BP_3DWidgetController)
	{
		WidgetController->StartGame();
	}
}

// void UStartMenuWidget::StartMusicBackground()
// {
// 	if (BackgroundMusicAudioComponent->IsValidLowLevelFast()) {
// 		BackgroundMusicAudioComponent->SetSound(BackgroudMusicCue);
//
// 		// You can fade the sound in... 
// 		float startTime = 0;
// 		float volume = 1.0f;
// 		float fadeTime = 1.1f;
// 		// BackgroundMusicAudioComponent->FadeIn(fadeTime);
// 		BackgroundMusicAudioComponent->Play();
// 	}
// }
