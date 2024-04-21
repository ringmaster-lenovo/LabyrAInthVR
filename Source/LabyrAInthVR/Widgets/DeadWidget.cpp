// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadWidget.h"
#include "Components/Button.h"
#include "WidgetContainer.h"


void UDeadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		UButton* RestartButtonWidget = Cast<UButton>(RestartButton->GetWidgetFromName(TEXT("ButtonHit")));
		if (RestartButtonWidget)
		{
			RestartButtonWidget->OnClicked.AddDynamic(this, &UDeadWidget::OnRestartClicked);
		}
	}
}

void UDeadWidget::OnRestartClicked()
{
	
}