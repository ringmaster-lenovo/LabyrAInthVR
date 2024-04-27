// Fill out your copyright notice in the Description page of Project Settings.


#include "WinWidget.h"

#include "LoseWidget.h"
#include "WidgetController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UWinWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//JUST FOR TESTING
	// SetTime(302);

	if (RankingsButton)
	{
		UButton* RankingsButtonWidget = Cast<UButton>(RankingsButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (RankingsButtonWidget)
		{
			RankingsButtonWidget->OnClicked.AddDynamic(this, &UWinWidget::OnRankingsClicked);
		}
	}

	if (MainMenuButton)
	{
		UButton* MainMenuButtonWidget = Cast<UButton>(MainMenuButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (MainMenuButtonWidget)
		{
			MainMenuButtonWidget->OnClicked.AddDynamic(this, &UWinWidget::OnMainMenuClicked);
		}
	}
}

void UWinWidget::SetTime(int32 time)
{
	int32 currentMinutes = time / 60;
	int32 currentSeconds = time % 60;

	// Per assicurarsi che i numeri siano sempre visualizzati con due cifre
	FNumberFormattingOptions NumberFormat;
	NumberFormat.MinimumIntegralDigits = 2;  
	NumberFormat.MaximumIntegralDigits = 2;
	
	FText TimeFormat = FText::Format(NSLOCTEXT("WinWidget", "TimeFormat", "{0}:{1}"),
	FText::AsNumber(currentMinutes, &NumberFormat),
	FText::AsNumber(currentSeconds, &NumberFormat));
	
	TimeText->SetText(TimeFormat);
}

void UWinWidget::OnNextLevelClicked()
{
	WidgetController->NextLevelButtonClicked();
}

void UWinWidget::OnRankingsClicked()
{
	WidgetController->RankingsButtonClicked();
}

void UWinWidget::OnMainMenuClicked()
{
	WidgetController->MainMenuButtonClicked();
}