// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp.h"
#include <Kismet/GameplayStatics.h>

#include "LabyrAInthVR/Core/VRGameMode.h"

// Sets default values
APowerUp::APowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APowerUp::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APowerUp::TriggerFrozenStar()
{
	// AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	// if (GameMode == nullptr) return;
	// ASceneController* SceneController = GameMode->GetSceneController();
	// if (FreezableActors.Num() <= 0) return;
	//
	// for (const auto& FreezableActor : FreezableActors)
	// {
	// 	if (FreezableActor == nullptr) continue;
	// 	if (!FreezableActor->Implements<UFreezableActor>()) continue;
	// 	Cast<IFreezableActor>(FreezableActor)->Freeze(10.f);
	// }
}

