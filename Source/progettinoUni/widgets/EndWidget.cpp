// Fill out your copyright notice in the Description page of Project Settings.


#include "EndWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"

void UEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//RestartButton->OnClicked.AddUniqueDynamic(this, &UEndWidget::OnRestartClicked);
	
}

/*void UEndWidget::OnRestartClicked()
{
	if(BP_WidgetController)
	{
		WidgetController->RestartGame();
	}
}*/