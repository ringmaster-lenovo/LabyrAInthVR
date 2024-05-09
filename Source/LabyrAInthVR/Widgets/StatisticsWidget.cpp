// Fill out your copyright notice in the Description page of Project Settings.


#include "StatisticsWidget.h"

#include "PlayerStatsSubSystem.h"
#include "WidgetController.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Player/PlayerStatistics.h"
#include "LabyrAInthVR/Player/VRMainCharacter.h"

void UStatisticsWidget::NativeConstruct()
{
	Super::NativeConstruct();

    MainCharacter = (!IsValid(MainCharacter)) ? Cast<AMainCharacter>(GetOwningPlayerPawn()) : MainCharacter;
    if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetPlayerStatistics()))
    {
        UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("Main character or Player statistics not valid from widget"))
    }

    StartTimer(MainCharacter->GetPlayerStatistics()->GetLevelTimer());
}

void UStatisticsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetPlayerStatistics()))
    {
        UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("Main character or Player statistics not valid from widget"))
    }
    
    SetStatisticsValues(
        MainCharacter->GetPlayerStatistics()->GetStat<float>(Esm_Speed),
        MainCharacter->GetPlayerStatistics()->HasShield(),
        MainCharacter->GetWeaponDamage(),
        MainCharacter->GetPlayerStatistics()->GetStat<float>(Esm_Health) / MainCharacter->GetPlayerStatistics()->GetDefaultHealth());
}

void UStatisticsWidget::SetStatisticsValues(int SpeedValue, bool bHasShield, int DamageValue, float HealthPercentage)
{
    // if (CurrentTime)
    // {
    //     int32 currentMinutes = CurrentTime / 60;
    //     int32 currentSeconds = CurrentTime % 60;
    //
    //     FString MinutesText = FString::Printf(TEXT("%02d"), currentMinutes);
    //     FString SecondsText = FString::Printf(TEXT("%02d"), currentSeconds);
    //
    //     if (minutes)
    //     {
    //         minutes->SetText(FText::FromString(MinutesText));
    //     }
    //
    //     if (seconds)
    //     {
    //         seconds->SetText(FText::FromString(SecondsText));
    //     }
    // }
    
    if (speed)
    {
        FString SpeedText = FString::Printf(TEXT("Speed: %d"), SpeedValue);
        speed->SetText(FText::FromString(SpeedText));
    }

    if (damage)
    {
        FString DamageText = FString::Printf(TEXT("Damage: %d"), DamageValue);
        damage->SetText(FText::FromString(DamageText));
    }

	if (health)
    {
        health->SetPercent(HealthPercentage);
    }

    if (shield)
    {
        shield->SetVisibility(bHasShield ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UStatisticsWidget::UpdateTimer()
{
    --CurrentTimeInSeconds;

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
    if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UStatisticsWidget::UpdateTimer, TimerInterval, true, 1.0f);
    }
}

//Stop timer function to call when the pause is called
void UStatisticsWidget::StopTimer()
{
    if (!GetWorld()) return; // Ensure we have a valid world context

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    // CurrentTimeInSeconds = 0; // Optionally reset the timer
}