// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuContainer.h"

#include "WidgetController.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMenuContainer::AMenuContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMenuContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMenuContainer::PauseMenuOpened()
{
	AWidgetController* WidgetController = Cast<AWidgetController>(UGameplayStatics::GetActorOfClass(GetWorld(), AWidgetController::StaticClass()));
	WidgetController->OnPauseGamePressed();
}

void AMenuContainer::PauseMenuClosed()
{
	AWidgetController* WidgetController = Cast<AWidgetController>(UGameplayStatics::GetActorOfClass(GetWorld(), AWidgetController::StaticClass()));
	WidgetController->OnResumeGame();
}