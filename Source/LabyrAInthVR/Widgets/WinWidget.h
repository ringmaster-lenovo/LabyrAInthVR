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
	class UUserWidget* NextLevelButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* RestartButton;

	UFUNCTION()
	void OnNextLevelClicked();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void SetTime(int32 time);
	
};
