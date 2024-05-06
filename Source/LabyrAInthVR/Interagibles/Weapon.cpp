// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "LabyrAInthVR/Player/Main3DCharacter.h"

#include <Kismet/GameplayStatics.h>

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::AssignToPlayer()
{
	AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Character->SetEquippedWeapon(this);
}

