// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetContainer.h"

// #include "LobbyWidget.h"
#include "SettingsWidget.h"
#include "LobbyWidget.h"
#include "WidgetController.h"


// Sets default values
AWidgetContainer::AWidgetContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AWidgetContainer::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bIsInVR)  // Non-VR widgets should be displayed on the screen
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			LobbyWidget = CreateWidget<ULobbyWidget>(PlayerController, LobbyWidgetClass);
			LobbyWidget->WidgetContainer = this;
		}
	else{
			LobbyWidget = Cast<ULobbyWidget>(Widget->GetUserWidgetObject());
			if (!LobbyWidget) return;
			LobbyWidget->WidgetContainer = this;
			// }
			SettingsWidget = Cast<USettingsWidget>(Widget->GetUserWidgetObject());
			if (!SettingsWidget) return;
			SettingsWidget->WidgetContainer = this;

			LoadingWidget = Cast<ULoadingWidget>(Widget->GetUserWidgetObject());
			if (!LoadingWidget) return;
		    LoadingWidget->WidgetContainer = this;
		}
}

FString AWidgetContainer::ShowMainMenuUI()
{
	if (LobbyWidgetClass)
	{
		if (!bIsInVR)  // Non-VR widgets should be displayed on the screen
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			LobbyWidget = CreateWidget<ULobbyWidget>(PlayerController, LobbyWidgetClass);
			if (!LobbyWidget) return "No LobbyWidget found!";
			LobbyWidget->WidgetContainer = this;
			// set the background color of the widget
			// LobbyWidget->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
			LobbyWidget->AddToViewport(0);
		} else
		{
			Widget->SetWidgetClass(LobbyWidgetClass);
			LobbyWidget = Cast<ULobbyWidget>(Widget->GetUserWidgetObject());
			LobbyWidget->WidgetContainer = this;
			if (!LobbyWidget) return "No LobbyWidget found!";
		}
		return "";
	}
	else return "No LobbyWidgetClass set!";
}

FString AWidgetContainer::ShowSettings()
{
	if (SettingsWidgetClass)
	{
		Widget->SetWidgetClass(SettingsWidgetClass);
		SettingsWidget = Cast<USettingsWidget>(Widget->GetUserWidgetObject());
		if (!SettingsWidget) return "No SettingsWidget found!";
		if (!bIsInVR)
		{
			LobbyWidget->RemoveFromParent();
			SettingsWidget->AddToViewport(0);
		}
		return "";
	}
	else return "No SettingsWidgetClass set!";
}

FString AWidgetContainer::ShowRankings()
{
	return "Not implemented yet!";
}

FString AWidgetContainer::ShowLoadingUI()
{
	if (LoadingWidgetClass)
	{
		Widget->SetWidgetClass(LoadingWidgetClass);
		LoadingWidget = Cast<ULoadingWidget>(Widget->GetUserWidgetObject());
		if (!LoadingWidget) return "No LoadingWidget found!";
		if (!bIsInVR)
		{
			LobbyWidget->RemoveFromParent();
			LoadingWidget->AddToViewport(0);
		}
		return "";
	}
	else return "No LoadingWidgetClass set!";
}

void AWidgetContainer::NewGameButtonClicked() const
{
	WidgetController->NewGameButtonClicked();
}

FString AWidgetContainer::HideMainMenuUI()
{
	if (LobbyWidgetClass)
	{
		if (!bIsInVR)  // Non-VR widgets should be displayed on the screen
		{
			UUserWidget* AnyWidget = Cast<UUserWidget>(Widget->GetUserWidgetObject());
			if (!AnyWidget) return "No Widget found in Main Menu Container!";
			AnyWidget->RemoveFromParent();
		}
		Widget->SetWidgetClass(LobbyWidgetClass);
		return "";
	}
	return "No LobbyWidgetClass set!";
}
