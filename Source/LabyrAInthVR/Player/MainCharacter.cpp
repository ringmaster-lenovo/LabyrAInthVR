#include "MainCharacter.h"

#include "EnhancedInputComponent.h"
#include "PlayerStatistics.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/SpotLightComponent.h"

#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Pickups/BasePickup.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Character_Log);

// This is the main character class for the VR game mode. It handles the VR camera, the VR controllers, and the VR movement.
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerStats = CreateDefaultSubobject<UPlayerStatistics>(TEXT("PlayerStatistics"));
	
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetAttenuationRadius(500000.f);

	PawnNoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
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
	if (!IsValid(PlayerStats)) return;

	PlayerStats->MainCharacter = this;
}

void AMainCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainCharacter::ResetWeapon()
{
	if (!IsValid(EquippedWeapon)) return;
	EquippedWeapon->Destroy();
}

float AMainCharacter::GetWeaponDamage()
{
	if (!IsValid(EquippedWeapon)) return 0.f;
	return EquippedWeapon->GetDamage();
}

bool AMainCharacter::IsAlive()
{
	if (!IsValid(PlayerStats)) return false;

	return PlayerStats->IsAlive();
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(FlashlightInputAction, ETriggerEvent::Triggered, this, &ThisClass::ToggleFlashlight);
		
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &ThisClass::Sprint, true);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Canceled, this, &ThisClass::Sprint, false);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &ThisClass::Sprint, false);
		
		EnhancedInputComponent->BindAction(ShootInputAction, ETriggerEvent::Started, this, &ThisClass::Shoot, true);
		EnhancedInputComponent->BindAction(ShootInputAction, ETriggerEvent::Canceled, this, &ThisClass::Shoot, false);
		EnhancedInputComponent->BindAction(ShootInputAction, ETriggerEvent::Completed, this, &ThisClass::Shoot, false);
	}
}

void AMainCharacter::ReleasePickupObject()
{
	if (!IsValid(EquippedWeapon) || !IsValid(EquippedWeapon->GetPickup())) return;

	ABasePickup* AuxPickup = EquippedWeapon->GetPickup();
	
	EquippedWeapon->Destroy();
	bHasWeapon = false;

	if (!IsValid(AuxPickup)) return;
	
	AuxPickup->SetActorHiddenInGame(false);
	AuxPickup->SetActorEnableCollision(true);
	AuxPickup->SetActorLocation(GetActorLocation() + (GetActorForwardVector() * 100.f));
}

void AMainCharacter::Sprint(const FInputActionValue& Value, bool bSprint)
{
	if (!IsValid(PlayerStats)) return;

	PlayerStats->Sprint(bSprint);
}

void AMainCharacter::Shoot(const FInputActionValue& Value, bool bIsPressingShoot)
{
	if (!IsValid(EquippedWeapon)) return;

	EquippedWeapon->SetMainCharacter(this);
	EquippedWeapon->Shoot(bIsPressingShoot);
}

void AMainCharacter::ToggleFlashlight(const FInputActionValue& Value)
{
	if (!IsValid(Flashlight)) return;

	Flashlight->SetVisibility(!Flashlight->GetVisibleFlag());
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

	if (!IsValid(PlayerStats) || !IsAlive()) return;

	PlayerStats->ChangeStatFloat(Esm_Health, -Damage);
}