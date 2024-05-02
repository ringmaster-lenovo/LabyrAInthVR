// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap.h"
#include <Kismet/GameplayStatics.h>
#include "StatsChangerComponent.h"

// Sets default values
ATrap::ATrap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATrap::Freeze(int32 Time)
{
	IFreezableActor::Freeze(Time);
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
