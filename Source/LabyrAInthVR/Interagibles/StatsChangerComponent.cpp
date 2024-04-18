// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsChangerComponent.h"
#include "PlayerStatsSubSystem.h"
#include <Kismet/GameplayStatics.h>


FName parseStatistics(EStatsMapping Stat) {
	FName result = "";
	switch (Stat) {
		case EStatsMapping::Esm_Health:
			result= "Health";
			break;

		case EStatsMapping::Esm_Speed:
			result = "Speed";
			break;

		case EStatsMapping::Esm_Resistance:
			result = "Shield";
			break;
	}
	return result;
}

// Sets default values for this component's properties
UStatsChangerComponent::UStatsChangerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatsChangerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UStatsChangerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UStatsChangerComponent::ChangeStats(TArray<FStatsType> Stats)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UPlayerStatsSubSystem* PlayerStatisticsSubsystem = GameInstance->GetSubsystem<UPlayerStatsSubSystem>();
	for (int32 i = 0; i < Stats.Num(); i++) {
		PlayerStatisticsSubsystem->AddToCounter(parseStatistics(Stats[i].Stats), Stats[i].Bonus);
	}
	/*PlayerStatisticsSubsystem->AddToCounter(FName("Health"), BonusHealth);
	PlayerStatisticsSubsystem->AddToCounter(FName("Speed"), BonusSpeed);
	PlayerStatisticsSubsystem->AddToCounter(FName("Resistance"), BonusResistance);*/
}

