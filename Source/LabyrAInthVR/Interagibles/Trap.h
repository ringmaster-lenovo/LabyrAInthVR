// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "Trap.generated.h"

UCLASS()
class LABYRAINTHVR_API ATrap : public AActor, public IDamageableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrap();

	/** Sound to play on collect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USoundBase* CollectSound;

	/** Apply Malus on the character */
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void OnCollect(TArray<FStatsType> Stats, UStatsChangerComponent* StatsChanger);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
