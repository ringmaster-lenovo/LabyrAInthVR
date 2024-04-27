// Fill out your copyright notice in the Description page of Project Settings.


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
	
	EnableInput(GetWorld()->GetFirstPlayerController());

	InputComponent->BindAction("PauseGame", IE_Pressed, this, &AWidgetController::OnPauseGamePressed);

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
	if(GameState->IsLoggedIn())
	{
		ShowMainMenu();
	} else
	{
		ShowPromptingWidget();
	}
}

void AWidgetController::ShowMainMenu()
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

void AWidgetController::ShowPromptingWidget()
{
	if (PromptingWidgetClass)
	{
		if (bIsInVR)
		{
			WidgetContainer->Widget->SetWidgetClass(PromptingWidgetClass);
			PromptingWidget = Cast<UPromptingWidget>(WidgetContainer->Widget->GetUserWidgetObject());
			if(!PromptingWidget)
			{
				FString ErrorString = "No LobbyWidget!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
			PromptingWidget->WidgetController = this;
		} else
		{
			RemoveAllWidgets(GetWorld());
			
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			PromptingWidget = CreateWidget<UPromptingWidget>(PlayerController, PromptingWidgetClass);
			PromptingWidget->WidgetController = this;
			// set the background color of the widget
			// LobbyWidget->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
			PromptingWidget->AddToViewport(0);
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

void AWidgetController::ShowWinScreen(int32 TimeOnLevel)
{
	if (WinWidgetClass)
	{
		if (bIsInVR)
		{
			WidgetContainer->Widget->SetWidgetClass(WinWidgetClass);
			WinWidget = Cast<UWinWidget>(WidgetContainer->Widget->GetUserWidgetObject());
			if(!WinWidget)
			{
				FString ErrorString = "No WinWidget!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
			WinWidget->SetTime(TimeOnLevel);
			WinWidget->WidgetController = this;
		} else
		{
			RemoveAllWidgets(GetWorld());
			
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			WinWidget = CreateWidget<UWinWidget>(PlayerController, WinWidgetClass);
			WinWidget->SetTime(TimeOnLevel);
			WinWidget->WidgetController = this;
			WinWidget->AddToViewport(0);
		}
	}
}

void AWidgetController::ShowLoseScreen()
{
	if (LoseWidgetClass)
	{
		if (bIsInVR)
		{
			WidgetContainer->Widget->SetWidgetClass(LoseWidgetClass);
			LoseWidget = Cast<ULoseWidget>(WidgetContainer->Widget->GetUserWidgetObject());
			if(!LoseWidget)
			{
				FString ErrorString = "No WinWidget!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
			LoseWidget->WidgetController = this;
		} else
		{
			RemoveAllWidgets(GetWorld());
			
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			LoseWidget = CreateWidget<ULoseWidget>(PlayerController, LoseWidgetClass);
			LoseWidget->WidgetController = this;
			LoseWidget->AddToViewport(0);
			
		}
	}
}

void AWidgetController::MainMenuButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Main Menu Button Clicked!"));
	OnReturnToMainMenuEvent.Broadcast();
}

void AWidgetController::RestartButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Restart Button Clicked!"));
	OnRestartLevelEvent.Broadcast();
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
	FString PlayerName = GameState->GetPlayerName();
	if (GameState->IsLoggedIn())
	{
		TArray<int> Levels;
		TArray<int> Times;
		ULabyrAInthVRGameInstance::LoadGame(PlayerName, Levels, Times);
		if (Levels.Num() > 0)
		{
			for (int i = 0; i < Levels.Num(); i++)
			{
				UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Player: %s, Level: %d, Time: %d"), *PlayerName, Levels[i], Times[i]);
				//TODO: POPOLARE WIDGET
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

void AWidgetController::SendButtonClicked()
{
	ShowMainMenu();
}

void AWidgetController::OnPauseGamePressed()
{
	ABasePlayerController* PlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController && MenuWidgetClass)
	{
		if (!bIsInVR && PlayerController->InGame)
		{
			if (MenuWidget && MenuWidget->IsInViewport())
			{
				MenuWidget->RemoveFromViewport();
				MenuWidget = nullptr;
				ShowGameUI();
			}
			else
			{
				RemoveAllWidgets(GetWorld());
				
				MenuWidget = CreateWidget<UMenuWidget>(PlayerController, MenuWidgetClass);
				MenuWidget->WidgetController = this;
				MenuWidget->AddToViewport(0);
			}
		}
	}
	OnPauseEvent.Broadcast();
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

