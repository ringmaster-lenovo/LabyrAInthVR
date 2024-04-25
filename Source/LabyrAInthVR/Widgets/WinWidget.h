// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WinWidget.generated.h"

class AWidgetContainer;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API UWinWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	AWidgetContainer* WidgetContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TimeText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* RankingsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* MainMenuButton;

	UFUNCTION()
	void OnRankingsClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void SetTime(int32 time);
	
};
