// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadWidget.h"
#include "Components/Button.h"
#include "WidgetContainer.h"


void UDeadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		UButton* RestartButtonWidget = Cast<UButton>(RestartButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (RestartButtonWidget)
		{
			RestartButtonWidget->OnClicked.AddDynamic(this, &UDeadWidget::OnRestartClicked);
		}
	}

	if (MainMenuButton)
	{
		UButton* MainMenuButtonWidget = Cast<UButton>(MainMenuButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (MainMenuButtonWidget)
		{
			MainMenuButtonWidget->OnClicked.AddDynamic(this, &UDeadWidget::OnMainMenuClicked);
		}
	}
}

void UDeadWidget::OnRestartClicked()
{
	
}

void UDeadWidget::OnMainMenuClicked()
{
	
}