#include "MainCharacter.h"
#include "PlayerStatistics.h"


#include "BasePlayerController.h"
#include "PlayerStatsSubSystem.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Character_Log);

// This is the main character class for the VR game mode. It handles the VR camera, the VR controllers, and the VR movement.
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerStats = CreateDefaultSubobject<UPlayerStatistics>(TEXT("PlayerStatistics"));
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	//UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	//UPlayerStatsSubSystem* PlayerStatisticsSubsystem = GameInstance->GetSubsystem<UPlayerStatsSubSystem>();
	//PlayerStatisticsSubsystem->SetCounter("Health", Life);
	//bool found = true;
	//float value;
	//PlayerStatisticsSubsystem->GetStatNumberValue(FName("Health"), found, value);
	//UE_LOG(LogVR, Warning, TEXT("VITA INIZIALIZZATA A: %f"), value);
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(!IsValid(PlayerStats)) return;

	PlayerStats->MainCharacter = this;
}

bool AMainCharacter::IsAlive()
{
	if(!IsValid(PlayerStats)) return false;

	return PlayerStats->IsAlive();
}

void AMainCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMainCharacter::StartTimer()
{
	if(!IsValid(PlayerStats)) return;

	PlayerStats->StartRawTimer();
}

UPlayerStatistics* AMainCharacter::GetPlayerStatistics()
{
	return PlayerStats;
}

/*void AMainCharacter::PickupWeapon_Implementation()
{
	
}*/

void AMainCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                   AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LabyrAInthVR_Character_Log, Display, TEXT("%s -> Taken %f damage by: %s"), *GetName(), Damage, *DamageCauser->GetName())

	if(!IsValid(PlayerStats) || !IsAlive()) return;

	PlayerStats->ChangeStatFloat(Esm_Health, -Damage);
}

/*void AMainCharacter::ReceiveDamage(float Damage, AActor* DamageCauser)
{
	/*UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
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

	if (Life > 0) return;
	
	//TODO: PLAYER IS DEAD, WHAT TO DO?
	//Teleport to lobby, set lobby a true, fare widget "SEI MORTO" (passa per la GameMode, chiama evento)#1#
}*/
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
