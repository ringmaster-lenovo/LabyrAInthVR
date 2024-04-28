#include "PlayerStatistics.h"

#include "BasePlayerController.h"
#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
}

void UPlayerStatistics::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;

	UCharacterMovementComponent* CharacterMovementComponent = MainCharacter->GetCharacterMovement();
	FTimerManager& WorldTimerManager = GetWorld()->GetTimerManager();
	float Vel = CharacterMovementComponent->Velocity.Size();
	
	if(Vel > 0 && Vel <= WalkSpeed && !GetWorld()->GetTimerManager().IsTimerActive(FootstepsSoundWalkTimerHandle))
	{
		bIsRunning = false;
		WorldTimerManager.ClearTimer(FootstepsSoundRunTimerHandle);
		WorldTimerManager.SetTimer(FootstepsSoundWalkTimerHandle, this, &ThisClass::PlayFootstepSound, WalkSoundInterval, true);
	}
	else if (Vel > WalkSpeed && !GetWorld()->GetTimerManager().IsTimerActive(FootstepsSoundRunTimerHandle))
	{
		bIsRunning = true;
		WorldTimerManager.ClearTimer(FootstepsSoundWalkTimerHandle);
		WorldTimerManager.SetTimer(FootstepsSoundRunTimerHandle, this, &ThisClass::PlayFootstepSound, RunSoundInterval, true);
	}
	else if(Vel == 0)
	{
		bIsRunning = false;
		WorldTimerManager.ClearTimer(FootstepsSoundWalkTimerHandle);
		WorldTimerManager.ClearTimer(FootstepsSoundRunTimerHandle);
	}
}

void UPlayerStatistics::PlayFootstepSound()
{
	if(!IsValid(MainCharacter)) return;
	float Velocity = MainCharacter->GetCharacterMovement()->Velocity.Size();
	UGameplayStatics::PlaySound2D(this, bIsRunning ? FootstepsRun : FootstepsWalk);
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
		       TEXT("%s -> Changing timed Speed from %f to %f for %f seconds"), *GetName(), CurrentSpeed, CurrentSpeed + Amount, Time)
		CurrentSpeed += Amount;
		UE_LOG(LogTemp, Warning, TEXT("ChangeTimedStat"))
		UpdateSpeed(CurrentSpeed);
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
	if(!IsValid(MainCharacter) || !IsValid(GetWorld())) return;
	
	Health = DefaultHealth;
	CurrentSpeed = WalkSpeed;
	bHasShield = false;
	LevelTime = 0.f;
	MainCharacter->ResetWeapon();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UPlayerStatistics::SetSpeedModifier(float NewSpeedModifier)
{
	SpeedModifier = NewSpeedModifier;
	UE_LOG(LogTemp, Warning, TEXT("SetSpeedModifier"))
	UpdateSpeed(CurrentSpeed);
}

void UPlayerStatistics::Sprint(bool bSprint)
{
	if(!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;
	
	CurrentSpeed = (bSprint ? RunSpeed : WalkSpeed);
	
	UpdateSpeed(CurrentSpeed);
}

void UPlayerStatistics::UpdateTimer()
{
	LevelTime++;
	UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Level time: %f"), *GetName(), LevelTime)
}

void UPlayerStatistics::ResetToDefaultValue(EStatModifier Stat)
{
	switch (Stat)
	{
	case Esm_Health:
		break;
	case Esm_Speed:
		UE_LOG(LabyrAInthVR_PlayerStatistics_Log, Display, TEXT("%s -> Resetting Speed from %f to %f"), *GetName(),
		       CurrentSpeed, bIsRunning ? RunSpeed : WalkSpeed)
		CurrentSpeed = bIsRunning ? RunSpeed : WalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("ResetToDefaultValue"))
		UpdateSpeed(CurrentSpeed);
		break;
	default: ;
	}
}

void UPlayerStatistics::UpdateSpeed(float NewSpeed)
{
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetCharacterMovement())) return;
	
	MainCharacter->GetCharacterMovement()->MaxWalkSpeed = NewSpeed - SpeedModifier * NewSpeed;
}
