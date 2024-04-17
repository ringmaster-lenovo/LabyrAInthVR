// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LabyrAInthVR/Player/VRMainCharacter.h"
#include "StatisticsWidget.generated.h"

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API UStatisticsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* minutes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* seconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* health;

	UFUNCTION(BlueprintCallable)
	void SetStatisticsValues(int SpeedValue, int ArmorValue, int DamageValue, float healthPercentage);

	UFUNCTION(BlueprintCallable)
	void UpdateTimer();

	UFUNCTION(BlueprintCallable)
	void StartTimer(int32 InitialTimeInSeconds);

	UFUNCTION(BlueprintCallable)
	void StopTimer();

private:
    FTimerHandle TimerHandle;
    int32 CurrentTimeInSeconds = 0;
	
	
};
