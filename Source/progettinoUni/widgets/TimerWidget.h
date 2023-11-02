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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Seconds;

	UPROPERTY()
	class AWidgetController* WidgetController = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWidgetController> BP_WidgetController;

	virtual void NativeConstruct() override;
	
};
