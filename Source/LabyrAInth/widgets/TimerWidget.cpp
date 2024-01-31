// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SecondsInt >= 60)
	{
		MinutesInt = SecondsInt / 60;
		SecondsInt = SecondsInt % 60;
	}
	
}

void UTimerWidget::UpdateTimer()
{
	if (!bIsTimerActive) { return; }
	if (SecondsInt > 0)
	{
		SecondsInt--;
	}
	else if (SecondsInt == 0 && MinutesInt > 0)
	{
		MinutesInt--;
		SecondsInt = 59;
	}
	else if (SecondsInt == 0 && MinutesInt == 0)
	{
		WidgetController->ShowEndUI();
	}
}
