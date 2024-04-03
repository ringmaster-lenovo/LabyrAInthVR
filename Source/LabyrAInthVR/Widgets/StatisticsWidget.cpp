// Fill out your copyright notice in the Description page of Project Settings.


#include "StatisticsWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UStatisticsWidget::NativeConstruct()
{
	Super::NativeConstruct();
			
	StartTimer(65); //mocked starting time
	SetStatisticsValues(100, 20, 34, 0.68); //mocked stats
}

void UStatisticsWidget::SetStatisticsValues(int SpeedValue, int ArmorValue, int DamageValue, float healthPercentage)
{
    if(speed)
    {
        FString SpeedText = FString::Printf(TEXT("Speed: %d"), SpeedValue);
        speed->SetText(FText::FromString(SpeedText));
    }

    if(armor)
    {
        FString ArmorText = FString::Printf(TEXT("Armor: %d"), ArmorValue);
        armor->SetText(FText::FromString(ArmorText));
    }

    if(damage)
    {
        FString DamageText = FString::Printf(TEXT("Damage: %d"), DamageValue);
        damage->SetText(FText::FromString(DamageText));
    }

	if(health)
    {
        health->SetPercent(healthPercentage);
    }
}

void UStatisticsWidget::UpdateTimer()
{
    ++CurrentTimeInSeconds;

    int32 currentMinutes = CurrentTimeInSeconds / 60;
    int32 currentSeconds = CurrentTimeInSeconds % 60;

	FString MinutesText = FString::Printf(TEXT("%02d"), currentMinutes);
    FString SecondsText = FString::Printf(TEXT("%02d"), currentSeconds);

    if (minutes)
    {
        minutes->SetText(FText::FromString(MinutesText));
    }

    if (seconds)
    {
        seconds->SetText(FText::FromString(SecondsText));
    }
}

void UStatisticsWidget::StartTimer(int32 InitialTimeInSeconds)
{
    CurrentTimeInSeconds = InitialTimeInSeconds;
    const float TimerInterval = 1.0f; // Update every second
    if (!GetWorld()) return; // Ensure we have a valid world context before starting the timer

	UpdateTimer();
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStatisticsWidget::UpdateTimer, TimerInterval, true);
}

//Stop timer function to call when the pause is called
void UStatisticsWidget::StopTimer()
{
    if (!GetWorld()) return; // Ensure we have a valid world context

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    CurrentTimeInSeconds = 0; // Optionally reset the timer
}