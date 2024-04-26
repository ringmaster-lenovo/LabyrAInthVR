// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"


#include "BasePlayerController.h"
#include "PlayerStatsSubSystem.h"
#include "Engine/World.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Widgets/StatisticsWidget.h"

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
	UE_LOG(LabyrAInthVR_Player_Log, Warning, TEXT("VITA INIZIALIZZATA A: %f"), value);	
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
	UE_LOG(LabyrAInthVR_Player_Log, Display, TEXT("Player Health Before Damage: %f"), value);

	UE_LOG(LabyrAInthVR_Player_Log, Display, TEXT("Player received damage by: %s"), *DamageCauser->GetName())
	
	if (Shield)
	{
		DectivateShield();
		UE_LOG(LabyrAInthVR_Player_Log, Display, TEXT("Player received damage but has shield, shield is destroyed"))
		return;
	}
	
	Life -= Damage;
	
	PlayerStatisticsSubsystem->AddToCounter("Health", -1 * Damage);
	
	PlayerStatisticsSubsystem->GetStatNumberValue(FName("Health"), found, value);
	UE_LOG(LabyrAInthVR_Player_Log, Display, TEXT("Player Health After Damage: %f"), value);

	if (Life <= 0)  // Player has died
	{
		ABasePlayerController* PlayerController = Cast<ABasePlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->PlayerHasDied();
		}
		else
		{
			UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("PlayerController not found"));
		}
	}
}

void AMainCharacter::StartLevelTimer()
{
	//START CHRONOMETER
	if (!GetWorld()) return; // Ensure we have a valid world context before starting the timer
	GetWorld()->GetTimerManager().SetTimer(TimerOnLevelHandle, this, &AMainCharacter::UpdateTimeOnCurrentLevel, 1.0f, true);
}

void AMainCharacter::StopAllTimers()
{
	//STOP CHRONOMETER
	if (!GetWorld()) return; // Ensure we have a valid world context before stopping the timer
	GetWorld()->GetTimerManager().ClearTimer(TimerOnLevelHandle);
	GetWorld()->GetTimerManager().ClearTimer(SpeedTimerHandle);
}

void AMainCharacter::ChangeSpeed(double SpeedIncrement, int32 Timer)
{
	BaseSpeed *= SpeedIncrement;
	RunningSpeed *= SpeedIncrement;
	SpeedTimerGoesOff = Timer;
	GetWorld()->GetTimerManager().SetTimer(SpeedTimerHandle, this, &AMainCharacter::UpdateSpeedTimer, 1.0f, true);
}

void AMainCharacter::ResetStats()
{
	Life = 100;
	Shield = false;
	TimeOnCurrentLevel = 0;
	PlayerName = "";
	BaseSpeed = 400;
	RunningSpeed = 600;
}

void AMainCharacter::UpdateTimeOnCurrentLevel()
{
	++TimeOnCurrentLevel;
}

void AMainCharacter::UpdateSpeedTimer()
{
	++SpeedTimer;
	if (SpeedTimer >= SpeedTimerGoesOff)
	{
		BaseSpeed = 400;
		RunningSpeed = 600;
		GetWorld()->GetTimerManager().ClearTimer(SpeedTimerHandle);
	}
}

void AMainCharacter::StopLevelTimer()
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().ClearTimer(TimerOnLevelHandle);
}

bool AMainCharacter::IsTimerActive() const
{
	if (!GetWorld()) return false;

	return GetWorld()->GetTimerManager().IsTimerActive(TimerOnLevelHandle);
}
