// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoseWidget.generated.h"

class AWidgetController;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ULoseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	AWidgetController* WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LoseText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* RestartButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* MainMenuButton;

	UFUNCTION()
	void SetFocusToButton();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void SetLoseText(const FString& Text) const;
};