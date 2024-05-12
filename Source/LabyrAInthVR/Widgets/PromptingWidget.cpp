// Fill out your copyright notice in the Description page of Project Settings.


#include "PromptingWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "WidgetController.h"
#include "LabyrAInthVR/Core/LabyrAInthVRGameInstance.h"
#include "LabyrAInthVR/Core/VRGameMode.h"
#include "LabyrAInthVR/Core/VRGameState.h"


void UPromptingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (PlayerNameTextBox)
	{
		PlayerNameTextBox->SetKeyboardFocus();
	}


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

void UPromptingWidget::SetFocusToFirstInteractiveElement()
{
	if (PlayerNameTextBox)
		{
			PlayerNameTextBox->SetKeyboardFocus();
		}
}

void UPromptingWidget::OnSendClicked()
{
	if (PlayerNameTextBox)
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No World!"));
			OnWidgetSError.Broadcast();
			return;
		}
		AVRGameState* GameState = World->GetGameState<AVRGameState>();
		if (!GameState)
		{
			UE_LOG(LabyrAInthVR_Widget_Log, Error, TEXT("No GameState!"));
			OnWidgetSError.Broadcast();
			return;
		}
		FText PlayerName = PlayerNameTextBox->GetText();
		FString PlayerNameString = PlayerName.ToString();
		PlayerNameString = PlayerNameString.TrimStartAndEnd();
		//Check if player name already exists
		TArray<FString> PlayerNames;
		ULabyrAInthVRGameInstance::LoadPlayerNames(PlayerNames);

		if (SendButton->TextBlock->GetText().EqualTo(FText::FromString("Load Game")) || !PlayerNames.Contains(PlayerNameString))
		{
			//If the player name does not exists or the player wants to load previous games
			FString ErrorString = GameState->LoginPlayer(PlayerNameString);
			if (PlayerNameString.IsEmpty() || ErrorString != "")
			{
				// Check if ErrorText is valid and then make it visible
				if (ErrorText)
				{
					ErrorText->SetVisibility(ESlateVisibility::Visible);
				}
			}
			else {
				// Optionally hide the ErrorText if it was previously visible
				if (ErrorText)
				{
					ErrorText->SetVisibility(ESlateVisibility::Collapsed);
				}

				WidgetController->SendButtonClicked();
			

				// Log the name to the output log
				UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("Player Name: %s"), *PlayerNameString);
			} 
		} else
		{
			//If the player name already exists, asks for login confirmation
			if (ErrorText)
			{
				ErrorText->SetVisibility(ESlateVisibility::Visible);
				ErrorText->SetText(FText::FromString("Name already exists, do you want to load previous games?"));
			}
			if (SendButton)
			{
				// SendButton->ButtonText = FText::FromString("Load Games");
				SendButton->TextBlock->SetText(FText::FromString("Load Game"));
			}
		}
		
	}
	else
	{
		// Optionally log an error or handle the case where PlayerNameTextBox is not valid
		UE_LOG(LabyrAInthVR_Widget_Log, Warning, TEXT("PlayerNameTextBox is not valid."));
	}
}