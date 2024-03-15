// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetContainer.h"

// #include "LobbyWidget.h"
#include "SettingsWidget.h"
#include "LobbyWidget.h"


// Sets default values
AWidgetContainer::AWidgetContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AWidgetContainer::BeginPlay()
{
	Super::BeginPlay();

	LobbyWidget = Cast<ULobbyWidget>(Widget->GetUserWidgetObject());
	if(!LobbyWidget) return;
	
	// ChooseTableWidget->StartWidget("Poker", "ACTIVE", "1");
}


void AWidgetContainer::ShowSettings()
{
	if (SettingsWidgetClass)
	{
		Widget->SetWidgetClass(SettingsWidgetClass);
		SettingsWidget = Cast<USettingsWidget>(Widget->GetUserWidgetObject());
		if(!SettingsWidget) return;
	
		// SettingsWidget->StartTimer();
		// Buttons->SetWidgetClass(nullptr);
	
	}
}
void AWidgetContainer::ShowRankings()
{
// 	if (RankingsWidgetClass)
// 	{
// 		Widget->SetWidgetClass(RankingsWidgetClass);
// 		URankingsWidget* RankingsWidget = Cast<URankingsWidget>(Widget->GetUserWidgetObject());
// 		if(!RankingsWidget) return;
//
// 		RankingsWidget->StartWidget();
// 		Buttons->SetWidgetClass(nullptr);
// 		
// }
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("AWidgetController:BeginPlay -> RankingsWidgetClass NULL"));
// 	}
//
//
//
}




