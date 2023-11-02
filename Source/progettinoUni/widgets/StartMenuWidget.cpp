// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"

void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddUniqueDynamic(this, &UStartMenuWidget::OnStartClicked);
	
}

void UStartMenuWidget::OnStartClicked()
{
	if(BP_3DWidgetController)
	{
		WidgetController->RemoveStartUI();
	}
}