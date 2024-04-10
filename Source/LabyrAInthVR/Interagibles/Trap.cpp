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

void ATrap::OnCollect(TArray<FStatsType> Stats, UStatsChangerComponent* StatsChanger) {
	// Try and play the sound if specified
	if (CollectSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CollectSound, this->GetActorLocation());
	}
	StatsChanger->ChangeStats(Stats);
}

