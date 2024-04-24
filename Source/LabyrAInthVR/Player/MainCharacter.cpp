#include "MainCharacter.h"
#include "PlayerStatistics.h"
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

void AMainCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LabyrAInthVR_Character_Log, Display, TEXT("%s -> Taken %f damage by: %s"), *GetName(), Damage, *DamageCauser->GetName())

	if(!IsValid(PlayerStats) || !IsAlive()) return;

	PlayerStats->ChangeStat(Esm_Health, -Damage);
}

/*void AMainCharacter::ReceiveDamage(float Damage, AActor* DamageCauser)
{
	/*UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
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
	//Teleport to lobby, set lobby a true, fare widget "SEI MORTO" (passa per la GameMode, chiama evento)#1#
}*/