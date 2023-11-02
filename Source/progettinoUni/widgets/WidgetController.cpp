// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

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

	if (BP_StartWidget)
	{
		StartMenuWidget = CreateWidget<UStartMenuWidget>(World, BP_StartWidget);
		StartMenuWidget->WidgetController = this;
	}

	ShowStartUI();
	
}

void AWidgetController::ShowStartUI()
{
	if(StartMenuWidget)
	{
		StartMenuWidget->AddToViewport(0);
	}
}

void AWidgetController::RemoveStartUI()
{
	if(StartMenuWidget)
	{
		StartMenuWidget->RemoveFromViewport();
	}
}

// Called every frame
void AWidgetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
