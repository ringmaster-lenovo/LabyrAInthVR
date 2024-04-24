// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetContainer.h"

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
	
}

FString AWidgetContainer::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (WidgetClass)
	{
		Widget->SetWidgetClass(WidgetClass);
		return "";
	}
	else return "No WidgetClass set!";
}
