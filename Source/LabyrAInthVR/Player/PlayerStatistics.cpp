#include "PlayerStatistics.h"

#include "BasePlayerController.h"
#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_PlayerStatistics_Log);

UPlayerStatistics::UPlayerStatistics()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStatistics::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;
	DefaultSpeed = MainCharacter->GetCharacterMovement()->MaxWalkSpeed;
}

void UPlayerStatistics::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerStatistics::ChangeStatFloat(EStatModifier Stat, float Amount)
{
	switch (Stat)
	{
	case Esm_Health:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Changing untimed Health from %f to %f"),
		       *GetName(), Health, Health + Amount)
		Health += Amount;
		if (Health <= 0.f)
		{
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
	case Esm_Armor:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Changing untimed Armor from %s to %s"),
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
		       TEXT("%s -> Changing timed Speed from %f to %f for %f seconds"), *GetName(), Speed, Speed + Amount, Time)
		Speed += Amount;
		UpdateSpeed(Speed);
		Delegate.BindUObject(this, &ThisClass::ResetToDefaultValue, Esm_Speed);
		GetWorld()->GetTimerManager().SetTimer(DefaultValueTimerHandle, Delegate, Time, false);
		break;
	default: ;
	}
}

void UPlayerStatistics::StartLevelTimer()
{
	const float TimerInterval = 1.0f;

	if (!IsValid(GetWorld())) return;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateTimer, TimerInterval, true);
}

void UPlayerStatistics::StopLevelTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	LevelTime = 0.f;
}

float UPlayerStatistics::GetLevelTime()
{
	return LevelTime;
}

float UPlayerStatistics::GetDefaultHealth()
{
	return DefaultHealth;
}

float UPlayerStatistics::GetCurrentWeaponDamage()
{
	if(!IsValid(MainCharacter)) return 0.f;

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

void UPlayerStatistics::ResetStats()
{
	Health = DefaultHealth;
	Speed = DefaultSpeed;
	bHasShield = false;
	LevelTime = 0.f;
	MainCharacter->ResetWeapon();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UPlayerStatistics::UpdateTimer()
{
	LevelTime++;
}

void UPlayerStatistics::ResetToDefaultValue(EStatModifier Stat)
{
	switch (Stat)
	{
	case Esm_Health:
		break;
	case Esm_Speed:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Resetting Speed from %f to %f"), *GetName(),
		       Speed, DefaultSpeed)
		Speed = DefaultSpeed;
		UpdateSpeed(Speed);
		break;
	default: ;
	}
}

void UPlayerStatistics::UpdateSpeed(float NewSpeed)
{
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;

	MainCharacter->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
