// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"

class USoundCue;

void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddUniqueDynamic(this, &UStartMenuWidget::OnStartClicked);
	
}

void UStartMenuWidget::OnStartClicked()
{
	if(BP_3DWidgetController)
	{
		// StartMusicBackground();
		WidgetController->RemoveStartUI();
	}
}

void UStartMenuWidget::StartMusicBackground()
{
	/*
	// Load our Sound Cue for the propeller sound we created in the editor... note your path may be different depending
	// on where you store the asset on disk.
	static ConstructorHelpers::FObjectFinder<USoundCue> propellerCue(TEXT("'/Game/StarterContent/Audio/Starter_Music_Cue.Starter_Music_Cue'"));

	// Store a reference to the Cue asset - we'll need it later.
	propellerAudioCue = propellerCue.Object;
	// Create an audio component, the audio component wraps the Cue, and allows us to ineract with
	// it, and its parameters from code.
	propellerAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PropellerAudioComp"));
	// I don't want the sound playing the moment it's created.
	propellerAudioComponent->bAutoActivate = false; // don't play the sound immediately.
	// I want the sound to follow the pawn around, so I attach it to the Pawns root.
	propellerAudioComponent->AttachParent = RootComponent;
	// I want the sound to come from slighty in front of the pawn.
	propellerAudioComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	*/
}
