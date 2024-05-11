// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "WidgetContainer.h"
#include "LobbyWidget.h"
#include "DemoTooltipWidget.h"
#include "LoadLevelsWidget.h"
#include "SpeedWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "LabyrAInthVR/Core/LabyrAInthVRGameInstance.h"
#include "LabyrAInthVR/Core/VRGameMode.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Widget_Log);

AWidgetController::AWidgetController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
}

void AWidgetController::BeginPlay()
{
	Super::BeginPlay();
	
	EnableInput(GetWorld()->GetFirstPlayerController());

	InputComponent->BindAction("PauseGame", IE_Pressed, this, &AWidgetController::OnPauseGamePressed).bExecuteWhenPaused = true;

	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	bIsInVR = GameMode->IsInVR();
	
	// AWidgetContainer* FoundWidgetContainer = Cast<AWidgetContainer>(
	// 	UGameplayStatics::GetActorOfClass(GetWorld(), AWidgetContainer::StaticClass()));
	// if (FoundWidgetContainer)
	// {
	// 	WidgetContainer = FoundWidgetContainer;
	// 	WidgetContainer->bIsInVR = bIsInVR;
	// 	WidgetContainer->WidgetController = this;
	// }
	// else
	// {
	// 	UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("WidgetContainer not found!"));
	// 	OnWidgetSError.Broadcast();
	// }
	TArray<AActor*> FoundWidgets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWidgetContainer::StaticClass(), FoundWidgets);

	for (AActor* Actor : FoundWidgets)
	{
		AWidgetContainer* FoundWidgetContainer = Cast<AWidgetContainer>(Actor);
		if (FoundWidgetContainer)
		{
			if (FoundWidgetContainer->GetActorLabel().Contains("LobbyWidgetContainer"))
			{
				WidgetContainer = FoundWidgetContainer;
				WidgetContainer->bIsInVR = bIsInVR;
				WidgetContainer->WidgetController = this;
			}
			else if (FoundWidgetContainer->GetActorLabel().Contains("DemoWidgetContainer"))
			{
				WidgetContainerTooltip = FoundWidgetContainer;
			}
		}
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
	if (GameState->IsLoggedIn())
	{
		ShowMainMenu();
	} else
	{
		ShowPromptingWidget();
	}
}

void AWidgetController::ShowDemoTooltip()
{
	if(DemoTooltipWidgetClass)
	{
		DemoTooltipWidget = Cast<UDemoTooltipWidget>(WidgetContainerTooltip->Widget->GetUserWidgetObject());
		if(!DemoTooltipWidget)
		{
			FString ErrorString = "No DemoTooltipWidget!";
			UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
			OnWidgetSError.Broadcast();
		}
		if(!bIsInVR)
		{
			FText Text1 = FText::FromString(TEXT("- Press 'F' to switch the torch"));
			FText Text2 = FText::FromString(TEXT("- Press 'E' to pick the weapon on your left"));
			FText Text3 = FText::FromString(TEXT("- Shoot and kill the enemy in front of you"));
			FText Text4 = FText::FromString(TEXT("- Press 'SHIFT' to run through the portal"));
			DemoTooltipWidget->Text1->SetText(Text1);
			DemoTooltipWidget->Text2->SetText(Text2);
			DemoTooltipWidget->Text3->SetText(Text3);
			DemoTooltipWidget->Text4->SetText(Text4);
			DemoTooltipWidget->Text5->SetVisibility(ESlateVisibility::Collapsed);
			DemoTooltipWidget->Text6->SetVisibility(ESlateVisibility::Collapsed);
			DemoTooltipWidget->Text7->SetVisibility(ESlateVisibility::Collapsed);
		} else
		{
			FText Text1 = FText::FromString(TEXT("- Press 'X' to open the statistics on your "));
			FText Text2 = FText::FromString(TEXT("  left hand"));
			FText Text3 = FText::FromString(TEXT("- Press 'A' to open the menu on your right hand"));
			FText Text4 = FText::FromString(TEXT("- Press 'B' to switch the torch"));
			FText Text5 = FText::FromString(TEXT("- Grab the weapon on your right"));
			FText Text6 = FText::FromString(TEXT("- Shoot and kill the enemy in front of you"));
			FText Text7 = FText::FromString(TEXT("- Press the right thumbstick to run "));
			FText Text8 = FText::FromString(TEXT("  through the portal"));
			DemoTooltipWidget->Text1->SetText(Text1);
			DemoTooltipWidget->Text2->SetText(Text2);
			DemoTooltipWidget->Text3->SetText(Text3);
			DemoTooltipWidget->Text4->SetText(Text4);
			DemoTooltipWidget->Text5->SetText(Text5);
			DemoTooltipWidget->Text6->SetText(Text6);
			DemoTooltipWidget->Text7->SetText(Text7);
			DemoTooltipWidget->Text8->SetText(Text8);
		}
	}
}

void AWidgetController::ShowMainMenu()
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
		if (bIsInVR)
		{
			if (VRLobbyWidgetClass)
			{
				WidgetContainer->Widget->SetWidgetClass(VRLobbyWidgetClass);
				LobbyWidget = Cast<ULobbyWidget>(WidgetContainer->Widget->GetUserWidgetObject());
				if(!LobbyWidget)
				{
					FString ErrorString = "No LobbyWidget!";
					UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
					OnWidgetSError.Broadcast();
				}
				FString PlayerLogged = GameState->GetPlayerName();
				FText PlayerText = FText::FromString(PlayerLogged);
				FText WelcomeText = FText::Format(FText::FromString(TEXT("Welcome {0}!")), PlayerText);
				LobbyWidget->WelcomeText->SetText(WelcomeText);
				LobbyWidget->WidgetController = this;
			} else {
				FString ErrorString = "No VRLobbyWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
		} else
		{
			if (LobbyWidgetClass)
			{
				RemoveAllWidgets(GetWorld());
			
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			
				LobbyWidget = CreateWidget<ULobbyWidget>(PlayerController, LobbyWidgetClass);
				LobbyWidget->WidgetController = this;
				FString PlayerLogged = GameState->GetPlayerName();
				FText PlayerText = FText::FromString(PlayerLogged);
				FText WelcomeText = FText::Format(FText::FromString(TEXT("Welcome {0}!")), PlayerText);
				LobbyWidget->WelcomeText->SetText(WelcomeText);
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
				// set the background color of the widget
				// LobbyWidget->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
				LobbyWidget->AddToViewport(0);
			} else {
				FString ErrorString = "No LobbyWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
		}
}

void AWidgetController::ShowPromptingWidget()
{
		if (bIsInVR)
		{
			if (VRPromptingWidgetClass)
			{
				WidgetContainer->Widget->SetWidgetClass(VRPromptingWidgetClass);
				PromptingWidget = Cast<UPromptingWidget>(WidgetContainer->Widget->GetUserWidgetObject());
				if(!PromptingWidget)
				{
					FString ErrorString = "No LobbyWidget!";
					UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
					OnWidgetSError.Broadcast();
				}
				PromptingWidget->WidgetController = this;
			} else {
				FString ErrorString = "No VRPromptingWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
		} else
		{
			if(PromptingWidgetClass)
			{
				RemoveAllWidgets(GetWorld());
			
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				PromptingWidget = CreateWidget<UPromptingWidget>(PlayerController, PromptingWidgetClass);
				PromptingWidget->WidgetController = this;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(PromptingWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
				// set the background color of the widget
				// LobbyWidget->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
				PromptingWidget->AddToViewport(0);
			} else
			{
				FString ErrorString = "No PromptingWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
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
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
	{
		UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No PlayerController!"));
		OnWidgetSError.Broadcast();
		return;
	}
	if (!bIsInVR)
	{
		RemoveAllWidgets(GetWorld());
		
		if (StatisticsWidgetClass)
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = false;
			StatisticsWidget = CreateWidget<UStatisticsWidget>(PlayerController, StatisticsWidgetClass);
			if (StatisticsWidget)
			{
				StatisticsWidget->AddToViewport(1);
			}
		}
	}  
	DamageWidget = CreateWidget<UDamageWidget>(PlayerController, DamageWidgetClass);
	DamageWidget->AddToViewport(0);
}

void AWidgetController::ShowWinScreen(const int32 TimeOnLevel)
{
		if (bIsInVR)
		{
			if(VRWinWidgetClass)
			{
				if (DamageWidget)
				{
					DamageWidget->RemoveFromParent();
				}
				WidgetContainer->Widget->SetWidgetClass(VRWinWidgetClass);
				WinWidget = Cast<UWinWidget>(WidgetContainer->Widget->GetUserWidgetObject());
				if (!WinWidget)
				{
					FString ErrorString = "No WinWidget!";
					UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
					OnWidgetSError.Broadcast();
				}
				WinWidget->SetTime(TimeOnLevel);
				WinWidget->WidgetController = this;
			} else {
				FString ErrorString = "No VRWinWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
		} else
		{
			if (WinWidgetClass)
			{
				RemoveAllWidgets(GetWorld());
			
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				WinWidget = CreateWidget<UWinWidget>(PlayerController, WinWidgetClass);
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(WinWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
				WinWidget->SetTime(TimeOnLevel);
				WinWidget->WidgetController = this;
				WinWidget->AddToViewport(0);
			} else {
				FString ErrorString = "No WinWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
		}
}

void AWidgetController::ShowLoseScreen(const bool bIsPlayerDead)
{
	
		if (bIsInVR)
		{
			if(VRLoseWidgetClass)
			{
				if (DamageWidget)
				{
					DamageWidget->RemoveFromParent();
				}
				WidgetContainer->Widget->SetWidgetClass(VRLoseWidgetClass);
				LoseWidget = Cast<ULoseWidget>(WidgetContainer->Widget->GetUserWidgetObject());
				if (!LoseWidget)
				{
					FString ErrorString = "No WinWidget!";
					UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
					OnWidgetSError.Broadcast();
					return;
				}
				LoseWidget->WidgetController = this;
			} else {
				FString ErrorString = "No VRLobbyWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
			
		} else
		{
			if (LoseWidgetClass)
			{
				RemoveAllWidgets(GetWorld());
			
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				LoseWidget = CreateWidget<ULoseWidget>(PlayerController, LoseWidgetClass);
				LoseWidget->WidgetController = this;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LoseWidget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
				LoseWidget->AddToViewport(0);
			} else {
				FString ErrorString = "No LoseWidgetClass set!";
				UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("%s"), *ErrorString);
				OnWidgetSError.Broadcast();
			}
		}
		if (bIsPlayerDead)
		{
			LoseWidget->SetLoseText("You Died!");
		} else
		{
			LoseWidget->SetLoseText("You Lost!");
		}
}

void AWidgetController::MainMenuButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Main Menu Button Clicked!"));
	OnReturnToMainMenuEvent.Broadcast();
}

void AWidgetController::RestartButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Restart Button Clicked!"));
	OnRestartLevelEvent.Broadcast();
}

void AWidgetController::StartNewGameButtonClicked() const
{
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Play Game Button Clicked!"));
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
	GetWorld()->GetGameState<AVRGameState>()->SetCurrentLevel(1);
	
	OnPlayGameButtonClicked.Broadcast();
}

void AWidgetController::ReplayContinueButtonClicked() 
{
	
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Replay/Continue Button Clicked!"));

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
			RemoveAllWidgets(World);
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if(LoadLevelsWidgetClass)
			{
				LoadLevelsWidget = CreateWidget<ULoadLevelsWidget>(PlayerController, LoadLevelsWidgetClass);
				LoadLevelsWidget->WidgetController = this;
				if (LoadLevelsWidget->LevelsBox)
				{
					for (int i = 0; i < Levels.Num(); i++)
					{
						// Log for debugging
						UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Player: %s, Level: %d, Time: %d"), *PlayerName, Levels[i], Times[i]);
						int32 currentMinutes =  Times[i] / 60;
						int32 currentSeconds =  Times[i] % 60;

						FText MinutesText =  FText::FromString(FString::Printf(TEXT("%02d"), currentMinutes));
						FText SecondsText = FText::FromString(FString::Printf(TEXT("%02d"), currentSeconds));
        
						// Create an instance of ButtonWidget for each level
						UButtonWidget* LoadLevelButton = CreateWidget<UButtonWidget>(PlayerController, ButtonWidgetClass);
						LoadLevelButton->Level = Levels[i];
						LoadLevelButton->OnClickedDelegate.AddUniqueDynamic(this, &AWidgetController::OnLevelButtonClicked);
						if (LoadLevelButton && LoadLevelButton->TextBlock)
						{
							// Setup the button properties
							FText FormattedText = FText::Format(FText::FromString(TEXT("Level {0}-Best Time {1}:{2} ")), FText::AsNumber(Levels[i]),MinutesText,SecondsText);
							LoadLevelButton->ButtonText = FormattedText;
							
							// Add the button to the VerticalBox
							LoadLevelsWidget->LevelsBox->AddChildToVerticalBox(LoadLevelButton);
						}
					}
					UButtonWidget* NewLevelButton = CreateWidget<UButtonWidget>(PlayerController, ButtonWidgetClass);
					NewLevelButton->Level = Levels.Num() + 1;
					NewLevelButton->OnClickedDelegate.AddUniqueDynamic(this, &AWidgetController::OnLevelButtonClicked);
					if (NewLevelButton && NewLevelButton->TextBlock)
					{
						// Setup the button properties
						FText NextLevelText = FText::FromString(TEXT("Next Level"));
						NewLevelButton->ButtonText = NextLevelText;
					}
					LoadLevelsWidget->LevelsBox->AddChildToVerticalBox(NewLevelButton);
					
				}
				if (bIsInVR)
				{
					WidgetContainer->Widget->SetWidgetClass(LoadLevelsWidgetClass);
					WidgetContainer->Widget->SetWidget(LoadLevelsWidget);
				} else
				{
					LoadLevelsWidget->AddToViewport(0);

				}
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
			UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Player has not played the game yet!"));
			return;
		}
	}
}

void AWidgetController::PlayDemoButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Play Demo Button Clicked!"));
	OnPlayDemoButtonClicked.Broadcast();
}

void AWidgetController::OnLevelButtonClicked(UButtonWidget* Button)
{
	LoadLevel(Button->Level);
}

void AWidgetController::LoadLevel(int8 Level)
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
	GameState->SetCurrentLevel(Level);
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Selected Level %d "), Level);

	OnPlayGameButtonClicked.Broadcast();
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

void AWidgetController::SettingsButtonClicked()
{
	UE_LOG(LabyrAInthVR_Widget_Log, Display, TEXT("Settings Button Clicked!"));
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
		if (PlayerController->InGame)
		{
			if(!bIsInVR)
			{
				if (MenuWidget && MenuWidget->IsInViewport())
				{
					MenuWidget->RemoveFromParent();
					MenuWidget = nullptr;
					ShowGameUI();
					OnResumeGameEvent.Broadcast();
				}
				else
				{
					RemoveAllWidgets(GetWorld());
				
					MenuWidget = CreateWidget<UMenuWidget>(PlayerController, MenuWidgetClass);
					MenuWidget->WidgetController = this;
					FInputModeGameAndUI InputMode;
					InputMode.SetWidgetToFocus(MenuWidget->TakeWidget());
					InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					
					PlayerController->SetInputMode(InputMode);
					PlayerController->bShowMouseCursor = true;
					MenuWidget->AddToViewport(0);
					OnPauseGameEvent.Broadcast();
				}
			} else
			{
				OnPauseGameEvent.Broadcast();
			}
			
		}
	}
}

void AWidgetController::OnResumeGame()
{
	OnResumeGameEvent.Broadcast();
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

void AWidgetController::ClearStatisticsTimer()
{
	if (StatisticsWidget)
	{
		StatisticsWidget->StopTimer();
	}
}


void AWidgetController::SetSpeedWidget(float Timer)
{
	if(SpeedWidget==nullptr || (IsValid(SpeedWidget) && !SpeedWidget->IsInViewport()))
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		SpeedWidget = CreateWidget<USpeedWidget>(PlayerController, SpeedWidgetClass);
		SpeedWidget->AddToViewport();
	}
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ThisClass::RemoveSpeedWidget);
	GetWorld()->GetTimerManager().SetTimer(RemoveSlowWidgetTimerHandle, Delegate, Timer, false);
}

void AWidgetController::RemoveSpeedWidget()
{
	if(IsValid(SpeedWidget) && SpeedWidget->IsInViewport())
	{
		SpeedWidget->RemoveFromParent();
	}
}

void AWidgetController::SetSlowWidget()
{
	if(SlowWidget==nullptr || (IsValid(SlowWidget) && !SlowWidget->IsInViewport()))
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		SlowWidget = CreateWidget<USlowWidget>(PlayerController, SlowWidgetClass);
		SlowWidget->AddToViewport();
	}
}

void AWidgetController::RemoveSlowWidget()
{
	if(IsValid(SlowWidget) && SlowWidget->IsInViewport())
	{
		SlowWidget->RemoveFromParent();
	}
}

