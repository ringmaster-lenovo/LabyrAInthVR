// Fill out your copyright notice in the Description page of Project Settings.


#include "LoseWidget.h"
#include "Components/Button.h"
#include "WidgetContainer.h"
#include "WidgetController.h"
#include "Components/TextBlock.h"


void ULoseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		UButton* RestartButtonWidget = Cast<UButton>(RestartButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (RestartButtonWidget)
		{
			RestartButtonWidget->OnClicked.AddDynamic(this, &ULoseWidget::OnRestartClicked);
		}
	}

	if (MainMenuButton)
	{
		UButton* MainMenuButtonWidget = Cast<UButton>(MainMenuButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (MainMenuButtonWidget)
		{
			MainMenuButtonWidget->OnClicked.AddDynamic(this, &ULoseWidget::OnMainMenuClicked);
		}
	}
}

void ULoseWidget::SetFocusToButton()
{
	if (RestartButton)
	{
		RestartButton->bIsFocusable = true;
		RestartButton->SetKeyboardFocus();
	}
}

void ULoseWidget::OnRestartClicked()
{
	WidgetController->RestartButtonClicked();
}

void ULoseWidget::OnMainMenuClicked()
{
	WidgetController->MainMenuButtonClicked();
}

void ULoseWidget::SetLoseText(const FString& Text) const
{
	LoseText->SetText(FText::FromString(Text));
}
