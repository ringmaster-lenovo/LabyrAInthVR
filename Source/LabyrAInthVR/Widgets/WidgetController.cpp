// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "WidgetContainer.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AWidgetController::AWidgetController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWidgetController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWidgetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWidgetController::ShowLoadingScreen()
{
	
	TArray<AActor*> FoundWidgetContainers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWidgetContainer::StaticClass(), FoundWidgetContainers);

	if (FoundWidgetContainers.Num() > 0)
	{
		// Assuming you want the first found instance
		WidgetContainer = Cast<AWidgetContainer>(FoundWidgetContainers[0]);
		if (WidgetContainer)
		{
			// TODO: Change it with ShowLoadingUI
			WidgetContainer->ShowSettings();
		}
	}
}

void AWidgetController::ShowGameUI()
{
}

