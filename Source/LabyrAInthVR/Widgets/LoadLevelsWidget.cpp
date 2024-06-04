// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadLevelsWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"


void ULoadLevelsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		UButton* BackButtonWidget = Cast<UButton>(BackButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (BackButtonWidget)
		{
			BackButtonWidget->OnClicked.AddDynamic(this, &ULoadLevelsWidget::OnBackClicked);
		}
	}
}

void ULoadLevelsWidget::SetFocusToButton()
{
	if (BackButton)
	{
		BackButton->bIsFocusable = true;
		BackButton->SetKeyboardFocus();
	}
}

void ULoadLevelsWidget::OnBackClicked()
{
	WidgetController->ShowMainMenu();
}
