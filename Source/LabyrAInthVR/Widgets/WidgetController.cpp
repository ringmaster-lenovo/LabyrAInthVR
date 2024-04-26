﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "WidgetContainer.h"
#include "LobbyWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/LabyrAInthVRGameInstance.h"
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

void AWidgetController::ShowLobbyUI()
{
	if (LobbyWidgetClass)
	{
		if (bIsInVR)
		{
			WidgetContainer->Widget->SetWidgetClass(LobbyWidgetClass);
			LobbyWidget = Cast<ULobbyWidget>(WidgetContainer->Widget->GetUserWidgetObject());
			if(!LobbyWidget)
			{
				FString ErrorString = "No LobbyWidget!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
			LobbyWidget->WidgetController = this;
		} else
		{
			RemoveAllWidgets(GetWorld());
			
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			LobbyWidget = CreateWidget<ULobbyWidget>(PlayerController, LobbyWidgetClass);
			LobbyWidget->WidgetController = this;
			// set the background color of the widget
			// LobbyWidget->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
			LobbyWidget->AddToViewport(0);
		}
	} else
	{
		FString ErrorString = "No LobbyWidgetClass set!";
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
		OnWidgetSError.Broadcast();
	}
}

void AWidgetController::ShowLoadingScreen()
{
	if (bIsInVR)
	{
		FString ErrorString = WidgetContainer->ShowWidget(LoadingWidgetClass);
		if (ErrorString != "")
		{
			UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
			OnWidgetSError.Broadcast();
		}
	} else
	{
		RemoveAllWidgets(GetWorld());
		if (LoadingWidgetClass)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			LoadingWidget = CreateWidget<ULoadingWidget>(PlayerController, LoadingWidgetClass);
			LoadingWidget->AddToViewport(0);
		}
	}
}

void AWidgetController::ShowGameUI()
{
	if (!bIsInVR)
	{
		RemoveAllWidgets(GetWorld());
		if (StatisticsWidgetClass)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			StatisticsWidget = CreateWidget<UStatisticsWidget>(PlayerController, StatisticsWidgetClass);
			StatisticsWidget->AddToViewport(0);
		}
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

void AWidgetController::StartNewGameButtonClicked() const
{
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Play Game Button Clicked!"));
	// Gets the game state and sets the current level to 0
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No World!"));
		OnWidgetSError.Broadcast();
		return;
	}
	AVRGameState* GameState = World->GetGameState<AVRGameState>();
	if (!GameState)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No GameState!"));
		OnWidgetSError.Broadcast();
		return;
	}
	GetWorld()->GetGameState<AVRGameState>()->SetCurrentLevel(0);
	
	OnPlayGameButtonClicked.Broadcast();
}

void AWidgetController::ReplayContinueButtonClicked() const
{
	
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Replay/Continue Button Clicked!"));
	// TODO: should ask the Game State the list of levels and times of the player
	// if the player has already played the game, there should be a list of levels and times
	// if the player has not played the game, the list should be empty so the click will be ignored
	// if the player has played the game, the player can choose to replay the previous level already beaten
	// or proceed to the next level

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No World!"));
		OnWidgetSError.Broadcast();
		return;
	}
	AVRGameState* GameState = World->GetGameState<AVRGameState>();
	if (!GameState)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No GameState!"));
		OnWidgetSError.Broadcast();
		return;
	}
	// FString PlayerName = GameState->GetPlayerName();
	FString PlayerName = "Player";
	if (!GameState->IsLoggedIn())
	{
		TArray<int> Levels;
		TArray<int> Times;
		ULabyrAInthVRGameInstance::LoadGame(PlayerName, Levels, Times);
		if (Levels.Num() > 0)
		{
			for (int i = 0; i < Levels.Num(); i++)
			{
				UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Player: %s, Level: %d, Time: %d"), *PlayerName, Levels[i], Times[i]);
			}
			// the player has already played the game
			// the player can choose to replay the previous level already beaten
			// or proceed to the next level
			// GameState->SetCurrentLevel(IL LIVELLO CORRISPONDENTE AL BOTTONE CLICCATO);
			// OnPlayGameButtonClicked.Broadcast();
		}
		else
		{
			// the player has not played the game, the click will be ignored
			UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Player has not played the game yet!"));
			return;
		}
	}

	// MOCKUP ONLY FOR TESTING
	NextLevelButtonClicked();
}

void AWidgetController::NextLevelButtonClicked() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No World!"));
		OnWidgetSError.Broadcast();
		return;
	}
	AVRGameState* GameState = World->GetGameState<AVRGameState>();
	if (!GameState)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No GameState!"));
		OnWidgetSError.Broadcast();
		return;
	}
	GameState->SetCurrentLevel(GameState->GetCurrentLevel() + 1);
	
	OnPlayGameButtonClicked.Broadcast();
}

void AWidgetController::RankingsButtonClicked() const
{
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Rankings Button Clicked!"));
	OnRankingsButtonClicked.Broadcast();
}

void AWidgetController::SettingsButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Settings Button Clicked!"));
	// TODO: should open the settings widget
}

void AWidgetController::QuitButtonClicked() const
{
	OnQuitGameButtonClicked.Broadcast();
}


void AWidgetController::RemoveAllWidgets(UObject* WorldContextObject)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(WorldContextObject, FoundWidgets, UUserWidget::StaticClass(), true);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (Widget && Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}
	}
}

