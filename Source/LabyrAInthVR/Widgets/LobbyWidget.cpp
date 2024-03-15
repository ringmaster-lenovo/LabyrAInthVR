// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

// #include "WidgetContainer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
			
	NewGame->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnNewGameClicked);
	LoadGame->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnLoadGameClicked);
	Rankings->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnRankingsClicked);
	Settings->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnSettingsClicked);
	Quit->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnQuitClicked);

	// TArray<AActor*> FoundActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWidgetContainer::StaticClass(), FoundActors);
    
	// if (FoundActors.Num() > 0)
	// {
	// 	// Assumiamo che vogliamo la prima istanza trovata
	// 	WidgetContainer = Cast<AWidgetContainer>(FoundActors[0]);
	// }
}

void ULobbyWidget::OnNewGameClicked()
{
	
}

void ULobbyWidget::OnLoadGameClicked()
{
	
}

void ULobbyWidget::OnRankingsClicked()
{
	
}

void ULobbyWidget::OnSettingsClicked()
{
	// WidgetContainer->ShowSettings();
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