// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "WidgetContainer.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/VRGameMode.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Widget_Log);

AWidgetController::AWidgetController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWidgetController::BeginPlay()
{
	Super::BeginPlay();

	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	bIsInVR = GameMode->IsInVR();
	
	AWidgetContainer* FoundWidgetContainer = Cast<AWidgetContainer>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AWidgetContainer::StaticClass()));
	if (FoundWidgetContainer)
	{
		WidgetContainer = FoundWidgetContainer;
		WidgetContainer->bIsInVR = bIsInVR;
		WidgetContainer->WidgetController = this;
	}
	else
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("WidgetContainer not found!"));
		OnWidgetSError.Broadcast();
	}
}

// Called every frame
void AWidgetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWidgetController::ShowMainMenu()
{	
	FString ErrorString = WidgetContainer->ShowMainMenuUI();
	if (ErrorString != "")
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
		OnWidgetSError.Broadcast();
	}
}

void AWidgetController::ShowLoadingScreen()
{
	FString ErrorString = WidgetContainer->ShowLoadingUI();
	if (ErrorString != "")
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
		OnWidgetSError.Broadcast();
	}
}

void AWidgetController::ShowGameUI()
{
	FString ErrorString = WidgetContainer->HideMainMenuUI();
	if (ErrorString != "")
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
		OnWidgetSError.Broadcast();
	}
}

void AWidgetController::ShowWinScreen()
{
	// TODO: Implement
	// the player wins the game and can choose to open the ranking widgets or return to the main menu
	OnReturnToMainMenuEvent.Broadcast();
}

void AWidgetController::ShowLoseScreen()
{
	// TODO: Implement
	// if the player loses he can choose whether to restart the level or return to the main menu
	// if he chooses to restart
	OnRestartLevelEvent.Broadcast();
	// if he chooses to return to the main menu
	// OnReturnToMainMenuEvent.Broadcast();
}

void AWidgetController::NewGameButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("New Game Button Clicked!"));
	OnNewGameButtonClicked.Broadcast();
}

