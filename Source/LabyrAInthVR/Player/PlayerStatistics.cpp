#include "PlayerStatistics.h"

#include "BasePlayerController.h"
#include "MainCharacter.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Music/MusicController.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_PlayerStatistics_Log);

UPlayerStatistics::UPlayerStatistics()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStatistics::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;
	MainCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	CurrentSpeed = WalkSpeed;
	Health = DefaultHealth;
}

void UPlayerStatistics::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;

	UCharacterMovementComponent* CharacterMovementComponent = MainCharacter->GetCharacterMovement();
	FTimerManager& WorldTimerManager = GetWorld()->GetTimerManager();
	const float Vel = CharacterMovementComponent->Velocity.Size();
	
	if (Vel == 0)
	{
		bIsRunning = false;
		WorldTimerManager.ClearTimer(FootstepsSoundWalkTimerHandle);
		WorldTimerManager.ClearTimer(FootstepsSoundRunTimerHandle);
	}
	if (!bIsRunning && !GetWorld()->GetTimerManager().IsTimerActive(FootstepsSoundWalkTimerHandle))
	{
		WorldTimerManager.ClearTimer(FootstepsSoundRunTimerHandle);
		WorldTimerManager.SetTimer(FootstepsSoundWalkTimerHandle, this, &ThisClass::PlayFootstepSound,
		                           WalkSoundInterval, true);
	}
	else if (bIsRunning && !GetWorld()->GetTimerManager().IsTimerActive(FootstepsSoundRunTimerHandle))
	{
		WorldTimerManager.ClearTimer(FootstepsSoundWalkTimerHandle);
		WorldTimerManager.SetTimer(FootstepsSoundRunTimerHandle, this, &ThisClass::PlayFootstepSound, RunSoundInterval,
		                           true);
	}
}

void UPlayerStatistics::PlayFootstepSound()
{
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetPawnNoiseEmitterComponent())) return;
	UGameplayStatics::PlaySound2D(this, bIsRunning ? FootstepsRun : FootstepsWalk);
	if (bIsRunning) MainCharacter->GetPawnNoiseEmitterComponent()->MakeNoise(
		MainCharacter, 1.0f, MainCharacter->GetActorLocation());
}

void UPlayerStatistics::ChangeStatFloat(EStatModifier Stat, float Amount)
{
	switch (Stat)
	{
	case Esm_Health:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Changing untimed Health from %f to %f"),
		       *GetName(), Health, Health + Amount)
		if (bHasShield && Amount < 0.f)
		{
			UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Shield is active, not taking damage"), *GetName())
			bHasShield = false;
			return;
		}
		Health = FMath::Min(GetDefaultHealth(), Health + Amount);  // Health can't go above DefaultHealth
		if (Health <= 0.f)
		{
			if (!IsValid(MainCharacter))
			{
				UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("MainCharacter not found, Player cannot die"));
			}
			ABasePlayerController* PlayerController = Cast<ABasePlayerController>(MainCharacter->GetController());

			if (PlayerController)
				PlayerController->PlayerHasDied();
			else
				UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("PlayerController not found"));
		}
		break;
	default: ;
	}
}

void UPlayerStatistics::ChangeStatBool(EStatModifier Stat, bool bEnable)
{
	switch (Stat)
	{
	case Esm_Shield:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Changing untimed Shield from %s to %s"),
		       *GetName(), bHasShield ? *FString("True") : *FString("False"),
		       bEnable ? *FString("True") : *FString("False"))
		bHasShield = bEnable;
		break;
	default: ;
	}
}

void UPlayerStatistics::ChangeTimedStat(EStatModifier Stat, float Amount, float Time)
{
	FTimerDelegate Delegate;

	switch (Stat)
	{
	case Esm_Speed:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display,
		       TEXT("%s -> Changing timed Speed from %f to %f for %f seconds"), *GetName(), CurrentSpeed,
		       CurrentSpeed + Amount, Time)
		SpeedPowerupModifier += Amount;
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("ChangeTimedStat"))
		UpdateSpeed();
		Delegate.BindUObject(this, &ThisClass::ResetThisPowerUpSpeedModifier, Esm_Speed, Amount);
		GetWorld()->GetTimerManager().SetTimer(DefaultValueTimerHandle, Delegate, Time, false);
		break;
	default: ;
	}
}

void UPlayerStatistics::StartLevelTimer()
{
	const float TimerInterval = 1.0f;

	if (!IsValid(GetWorld())) return;

	MusicController = Cast<AMusicController>(UGameplayStatics::GetActorOfClass(GetWorld(), AMusicController::StaticClass()));
	if (IsValid(MusicController)) MusicController->ResetVolumeMultiplier();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateTimer, TimerInterval, true, 1.f);
}

void UPlayerStatistics::UpdateTimer()
{
	LevelTime++;
	LevelTimer--;
	if (MusicController && LevelTimer < 60)
	{
		MusicController->PlayClockSound(LevelTimer);
	}
	if (LevelTimer <= 0)
	{
		if (!IsValid(MainCharacter))
		{
			UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("MainCharacter not found, Player cannot lose"));
		}
		ABasePlayerController* PlayerController = Cast<ABasePlayerController>(MainCharacter->GetController());
		if (PlayerController) PlayerController->PlayerTimerWentOff();
		else UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("PlayerController not found"));
	}
}

void UPlayerStatistics::StopLevelTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UPlayerStatistics::PauseLevelTimerForDuration(float PauseDuration)
{
	if (!GetWorld()) return;

	// First, stop the main timer.
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	// Set a new timer that will wait for 'PauseDuration' seconds before restarting the main timer.
	GetWorld()->GetTimerManager().SetTimer(PauseTimerHandle, this, &UPlayerStatistics::ResumeLevelTimer, PauseDuration, false);
}


void UPlayerStatistics::ResumeLevelTimer()
{
	if (!GetWorld()) return;

	// Clear any existing pause timer handle
	GetWorld()->GetTimerManager().ClearTimer(PauseTimerHandle);

	// Restart the main timer as before
	const float TimerInterval = 1.0f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerStatistics::UpdateTimer, TimerInterval, true);
	if(IsValid(MainCharacter)) {
		MainCharacter->SetIsFrozen(false);
	}
}



float UPlayerStatistics::GetLevelTime()
{
	return LevelTime;
}

float UPlayerStatistics::GetLevelTimer()
{
	return LevelTimer;
}

float UPlayerStatistics::GetDefaultHealth() const
{
	return DefaultHealth;
}

float UPlayerStatistics::GetCurrentWeaponDamage()
{
	if (!IsValid(MainCharacter)) return 0.f;

	return MainCharacter->GetWeaponDamage();
}

FPlayerTime UPlayerStatistics::GetPlayerTime()
{
	if (LevelTime <= 0.f) return FPlayerTime{0, 0, 0};

	int Minutes = LevelTime / 60.f;
	int Hours = Minutes / 60.f;

	Minutes -= Hours * 60.f;
	int Seconds = LevelTime - Minutes * 60.f;

	return FPlayerTime{Seconds, Minutes, Hours};
}

void UPlayerStatistics::ResetSpeed()
{
	CurrentSpeed = WalkSpeed;
	RunSpeedModifier = 0.f;
	SpeedPowerupModifier = 0.f;
	SpeedTrapModifier = 0.f;
}

void UPlayerStatistics::ResetStats()
{
	if (!IsValid(MainCharacter) || !IsValid(GetWorld())) return;

	Health = DefaultHealth;
	ResetSpeed();
	bHasShield = false;
	LevelTime = 0.f;
	MainCharacter->ResetWeapon();
	MainCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UPlayerStatistics::SetSpeedModifier(float NewSpeedModifier)
{
	SpeedTrapModifier += NewSpeedModifier;
	UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Warning, TEXT("SetSpeedModifier"))
	UpdateSpeed();
}

void UPlayerStatistics::Sprint(bool bSprint)
{
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;

	RunSpeedModifier = (bSprint ? BaseRunSpeedModifier : 0);
	bIsRunning = bSprint;
	UpdateSpeed();
}

void UPlayerStatistics::ResetThisPowerUpSpeedModifier(EStatModifier Stat, float Amount)
{
	switch (Stat)
	{
	case Esm_Speed:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Resetting Speed modifier from %f to %f"), *GetName(), SpeedPowerupModifier, 0.f)
		SpeedPowerupModifier -= Amount;
		UpdateSpeed();
		break;
	default: ;
	}
}

void UPlayerStatistics::UpdateSpeed()
{
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;

	CurrentSpeed = FMath::Max(50, WalkSpeed + RunSpeedModifier + SpeedPowerupModifier - SpeedTrapModifier);  // Speed can't go below 50
	MainCharacter->GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}
