// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "WidgetContainer.h"


void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Collega gli eventi dei pulsanti
	if (NewGameButton)
	{
		UButton* NewGameButtonWidget = Cast<UButton>(NewGameButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (NewGameButtonWidget)
		{
			NewGameButtonWidget->OnClicked.AddDynamic(this, &ULobbyWidget::OnNewGameButtonClicked);
		}
	}

	if (LoadGameButton)
	{
		UButton* LoadGameButtonWidget = Cast<UButton>(LoadGameButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (LoadGameButtonWidget)
		{
			LoadGameButtonWidget->OnClicked.AddDynamic(this, &ULobbyWidget::OnLoadGameClicked);
		}
	}

	if (RankingsButton)
	{
		UButton* RankingsButtonWidget = Cast<UButton>(RankingsButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (RankingsButtonWidget)
		{
			RankingsButtonWidget->OnClicked.AddDynamic(this, &ULobbyWidget::OnRankingsClicked);
		}
	}

	if (SettingsButton)
	{
		UButton* SettingsButtonWidget = Cast<UButton>(SettingsButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (SettingsButtonWidget)
		{
			SettingsButtonWidget->OnClicked.AddDynamic(this, &ULobbyWidget::OnSettingsClicked);
		}
	}

	if (QuitButton)
	{
		UButton* QuitButtonWidget = Cast<UButton>(QuitButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (QuitButtonWidget)
		{
			QuitButtonWidget->OnClicked.AddDynamic(this, &ULobbyWidget::OnQuitClicked);
		}
	}
}

void ULobbyWidget::OnNewGameButtonClicked()
{
	WidgetContainer->NewGameButtonClicked();
}

void ULobbyWidget::OnLoadGameClicked()
{
	
}

void ULobbyWidget::OnRankingsClicked()
{
	
}

void ULobbyWidget::OnSettingsClicked()
{
	WidgetContainer->ShowSettings();
}

void ULobbyWidget::OnQuitClicked()
{
	// Ottiene il World context
	UWorld* World = GetWorld();
	if (!World) return;

	// Ottiene il Player Controller
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	// Chiude il gioco. Cambia EQuitPreference::Quit a EQuitPreference::Backgrounding se vuoi che il gioco vada in background invece di chiudersi.
	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
}