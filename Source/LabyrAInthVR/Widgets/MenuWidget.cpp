// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "Components/Button.h"
#include "WidgetController.h"


void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Collega gli eventi dei pulsanti
	if (SettingsButton)
	{
		UButton* SettingsButtonWidget = Cast<UButton>(SettingsButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (SettingsButtonWidget)
		{
			SettingsButtonWidget->OnClicked.AddDynamic(this, &UMenuWidget::OnSettingsClicked);
		}
	}

	if (RestartButton)
	{
		UButton* RestartButtonWidget = Cast<UButton>(RestartButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (RestartButtonWidget)
		{
			RestartButtonWidget->OnClicked.AddDynamic(this, &UMenuWidget::OnRestartClicked);
		}
	}

	if (MainMenuButton)
	{
		UButton* MainMenuButtonWidget = Cast<UButton>(MainMenuButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (MainMenuButtonWidget)
		{
			MainMenuButtonWidget->OnClicked.AddDynamic(this, &UMenuWidget::OnMainMenuClicked);
		}
	}
}

void UMenuWidget::OnSettingsClicked()
{
	WidgetController->SettingsButtonClicked();
}

void UMenuWidget::OnRestartClicked()
{
	WidgetController->RestartButtonClicked();
}

void UMenuWidget::OnMainMenuClicked()
{
	WidgetController->MainMenuButtonClicked();
}