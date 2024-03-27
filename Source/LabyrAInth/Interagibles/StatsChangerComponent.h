// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsChangerComponent.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EStatsMapping {
	Esm_Health,
	Esm_Speed,
	Esm_Resistance
};

USTRUCT(Blueprintable, BlueprintType)
struct FStatsType{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	EStatsMapping Stats;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	float Bonus;
};



UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LABYRAINTH_API UStatsChangerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatsChangerComponent();

	/** Player health gain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	float BonusHealth=0;

	/** Player bonus resistance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	float BonusResistance=0;

	/** Player speed boost */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp")
	float BonusSpeed=0;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Change Character Stats
	UFUNCTION(BlueprintCallable, Category = "StatsChanger")
	virtual void ChangeStats(TArray<FStatsType> Stats);
};
