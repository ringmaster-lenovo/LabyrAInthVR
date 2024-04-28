// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"


void UButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UButton* LevelButtonWidget = Cast<UButton>(this->GetWidgetFromName(TEXT("ButtonHit")));
	if (LevelButtonWidget)
	{
		LevelButtonWidget->OnClicked.AddUniqueDynamic(this, &UButtonWidget::OnButtonClicked);
	}
}

void UButtonWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(this);
}