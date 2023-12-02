// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralFloor.h"

// Sets default values
AProceduralFloor::AProceduralFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComponent"));
	SetRootComponent(FloorMeshComponent);

}

// Called when the game starts or when spawned
void AProceduralFloor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralFloor::SpawnPillars(UClass* PillarClass)
{
	GetWorld()->SpawnActor<AActor>(PillarClass, FVector(0), FRotator(0));
}