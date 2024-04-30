// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PromptingWidget.generated.h"

class AWidgetController;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API UPromptingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	AWidgetController* WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* PlayerNameTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ErrorText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButtonWidget* SendButton;

	DECLARE_MULTICAST_DELEGATE(FWidgetsErrorEvent);
	FWidgetsErrorEvent OnWidgetSError;

	UFUNCTION()
	void OnSendClicked();
	
};
