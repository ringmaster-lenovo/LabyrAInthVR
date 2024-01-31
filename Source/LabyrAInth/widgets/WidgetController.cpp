// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "TimerWidget.h"
#include "EndWidget.h"
#include "StartMenuWidget.h"
#include "Components/TileView.h"

// Sets default values
AWidgetController::AWidgetController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AWidgetController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) { return; }

	if (BP_GenMapWidget)
	{
		GenMapWidget = CreateWidget<UGenMapWidget>(World, BP_GenMapWidget);
		GenMapWidget->WidgetController = this;
	}
	if (BP_StartWidget)
	{
		StartMenuWidget = CreateWidget<UStartMenuWidget>(World, BP_StartWidget);
		StartMenuWidget->WidgetController = this;
	}
	if (BP_EndWidget)
	{
		EndWidget = CreateWidget<UEndWidget>(World, BP_EndWidget);
		EndWidget->WidgetController = this;
	}
	if (BP_TimerWidget)
	{
		TimerWidget = CreateWidget<UTimerWidget>(World, BP_TimerWidget);
		TimerWidget->WidgetController = this;
	}
	if (BP_PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(World, BP_PauseMenuWidget);
		PauseMenuWidget->WidgetController = this;
	}
	PlayerController3D = Cast<A3DPlayerController>(GetWorld()->GetFirstPlayerController());

	// Start a repeating timer that calls UpdateTimer every second
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWidgetController::UpdateTimer, 1.0f, true, 1.0f);

	ShowGenMapUI();
	
}


void AWidgetController::ShowGenMapUI() const
{
	if (GenMapWidget)
	{
		GenMapWidget->AddToViewport(0);
	}
}

void AWidgetController::ShowStartUI() const
{
	if (GenMapWidget)
	{
		GenMapWidget->RemoveFromParent();
	}
	if (StartMenuWidget)
	{
		StartMenuWidget->AddToViewport(0);
	}
}

void AWidgetController::StartGame() const
{
	if (StartMenuWidget)
	{
		StartMenuWidget->RemoveFromParent();
	}
	if (TimerWidget)
	{
		TimerWidget->AddToViewport(0);
		TimerWidget->StartTimer();
	}
	if (PlayerController3D)
	{
		PlayerController3D->SetPlayerControllerEnabled(true);
	}
}

void AWidgetController::PauseGame() const
{
	if (PauseMenuWidget && TimerWidget)
	{
		PauseMenuWidget->AddToViewport(0);
		TimerWidget->SetVisibility(ESlateVisibility::Hidden);
		TimerWidget->StopTimer();
	}
	if (PlayerController3D)
	{
		PlayerController3D->SetPlayerControllerEnabled(false);
	}
}

void AWidgetController::ResumeGame() const
{
	if (PauseMenuWidget && TimerWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		TimerWidget->SetVisibility(ESlateVisibility::Visible);
		TimerWidget->StartTimer();
	}
	if (PlayerController3D)
	{
		PlayerController3D->SetPlayerControllerEnabled(true);
	}
}

void AWidgetController::QuitGame() const
{
	
}

void AWidgetController::ShowEndUI() const
{
	if (BP_EndWidget)
	{
		if (EndWidget->IsInViewport()) return;
		EndWidget->AddToViewport(0);
	}
	if (BP_TimerWidget)
	{
		TimerWidget->RemoveFromParent();
	}
	if (PlayerController3D)
	{
		PlayerController3D->SetPlayerControllerEnabled(false);
	}
}

void AWidgetController::RestartGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

UStartMenuWidget* AWidgetController::GetStartMenuWidget() const
{
	if (StartMenuWidget) return StartMenuWidget;
	else return nullptr;
}

void AWidgetController::SetAvailableTimeAmount(const int32& AvailableTimeAmount) const
{
	if (BP_TimerWidget)
	{
		TimerWidget->SetAvailableSeconds(AvailableTimeAmount);
	}
}

// Called every frame
void AWidgetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}