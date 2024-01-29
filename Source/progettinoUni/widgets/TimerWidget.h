// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROGETTINOUNI_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	bool bIsTimerActive = false;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Seconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	int32 MinutesInt = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	int32 SecondsInt = 0;

	UFUNCTION()
	void SetAvailableSeconds(const int32 SecondsAmount) {SecondsInt = SecondsAmount;}

	UPROPERTY()
	class AWidgetController* WidgetController = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWidgetController> BP_WidgetController;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateTimer();

	void StartTimer() {bIsTimerActive = true;}

	void StopTimer() {bIsTimerActive = false;}

	void ResetTimer() {MinutesInt = 0; SecondsInt = 0;}
	
};
