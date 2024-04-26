// Fill out your copyright notice in the Description page of Project Settings.


#include "PromptingWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "WidgetController.h"


void UPromptingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Collega gli eventi dei pulsanti
	if (SendButton)
	{
		UButton* SendButtonWidget = Cast<UButton>(SendButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (SendButtonWidget)
		{
			SendButtonWidget->OnClicked.AddDynamic(this, &UPromptingWidget::OnSendClicked);
		}
	}
}

void UPromptingWidget::OnSendClicked()
{
	if (PlayerNameTextBox)
	{
		FText PlayerName = PlayerNameTextBox->GetText();
		FString PlayerNameString = PlayerName.ToString();

		if (PlayerNameString.IsEmpty())
		{
			// Check if ErrorText is valid and then make it visible
			if (ErrorText)
			{
				ErrorText->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			// Optionally hide the ErrorText if it was previously visible
			if (ErrorText)
			{
				ErrorText->SetVisibility(ESlateVisibility::Collapsed);
			}

			WidgetController->SendButtonClicked();
			//TODO: Add PlayerName to GameState

			// Log the name to the output log
			UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *PlayerNameString);
		}
	}
	else
	{
		// Optionally log an error or handle the case where PlayerNameTextBox is not valid
		UE_LOG(LogTemp, Warning, TEXT("PlayerNameTextBox is not valid."));
	}
}