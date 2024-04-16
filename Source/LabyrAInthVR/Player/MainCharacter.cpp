// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"


#include "PlayerStatsSubSystem.h"
#include "Engine/World.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogVR);

// This is the main character class for the VR game mode. It handles the VR camera, the VR controllers, and the VR movement.
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World) { return; }
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UPlayerStatsSubSystem* PlayerStatisticsSubsystem = GameInstance->GetSubsystem<UPlayerStatsSubSystem>();
	PlayerStatisticsSubsystem->SetCounter("Health", Life);
	bool found = true;
	float value;
	PlayerStatisticsSubsystem->GetStatNumberValue(FName("Health"), found, value);
	UE_LOG(LogVR, Warning, TEXT("VITA INIZIALIZZATA A: %f"), value);
}

// Called every frame
void AMainCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMainCharacter::ReceiveDamage(float Damage, AActor* DamageCauser)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UPlayerStatsSubSystem* PlayerStatisticsSubsystem = GameInstance->GetSubsystem<UPlayerStatsSubSystem>();
	bool found = true;
	float value;
	PlayerStatisticsSubsystem->GetStatNumberValue(FName("Health"), found, value);
	UE_LOG(LogVR, Warning, TEXT("ECCO LA VITAAAA: %f"), value);

	if (Life == 0) return;

	UE_LOG(LogTemp, Warning, TEXT("Player received damage by: %s"), *DamageCauser->GetName())
	
	if(Shield)
	{
		DectivateShield();
		UE_LOG(LogTemp, Warning, TEXT("Player received damage but has shield, shield is destroyed"))
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Received damage"))
	// Life -= Damage;
	
	PlayerStatisticsSubsystem->AddToCounter("Health", -1 * Damage);
	
	PlayerStatisticsSubsystem->GetStatNumberValue(FName("Health"), found, value);
	UE_LOG(LogVR, Warning, TEXT("ECCO LA VITAAAA AFTER DAMAGE: %f"), value);

	if (Life > 0) return;
	
	//TODO: PLAYER IS DEAD, WHAT TO DO?
	//Teleport to lobby, set lobby a true, fare widget "SEI MORTO" (passa per la GameMode, chiama evento)
}