// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::OnOptionsClicked);
	}
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}
	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(this, &UPauseMenuWidget::OnQuitClicked);
	}
}

void UPauseMenuWidget::OnOptionsClicked()
{
	
}

void UPauseMenuWidget::OnResumeClicked()
{
	if (WidgetController)
	{
		WidgetController->ResumeGame();
	}
	
}

void UPauseMenuWidget::OnRestartClicked()
{
	
}

void UPauseMenuWidget::OnQuitClicked()
{
	if (WidgetController)
	{
		WidgetController->QuitGame();
	}
	
}
