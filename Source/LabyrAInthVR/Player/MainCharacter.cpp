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
