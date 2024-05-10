#include "Weapon.h"
#include "LabyrAInthVR/Player/Main3DCharacter.h"

#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "Particles/ParticleSystem.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(LabyrAInthVR_Weapon_Log);

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Shoot()
{
	if (!bCanShoot && bShouldDelayInput) return;
	
	ShootInternal();
	bCanShoot = false;
	
	if(bShouldDelayInput) GetWorldTimerManager().SetTimer(ShootingTimerHandle, this, &ThisClass::ResetShooting, FireRate,
		                                false);
}

void AWeapon::Destroyed()
{
	Super::Destroyed();
	if( GetWorldTimerManager().IsTimerActive(ShootingTimerHandle)) GetWorldTimerManager().ClearTimer(ShootingTimerHandle);
}

void AWeapon::ShootInternal()
{
	if (!IsValid(MuzzleEffect) || !IsValid(MainCharacter)) return;

	USkeletalMeshComponent* WeaponMesh = FindComponentByClass<USkeletalMeshComponent>();

	if (!IsValid(WeaponMesh)) return;

	FVector Start = WeaponMesh->GetSocketTransform(FName("Muzzle_Front")).GetLocation();

	FVector End = Start + (GetActorForwardVector() * 50000.f);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = MainCharacter;
	SpawnParameters.Owner = this;
	
	if (IsValid(FireAnimation)) WeaponMesh->PlayAnimation(FireAnimation, false);
	
	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, FName("Muzzle_Front"));
	
	AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass, Start, End.Rotation(), SpawnParameters);
	
	if (!IsValid(SpawnedProjectile)) return;
	
	SpawnedProjectile->SetDamage(Damage);

	if (!IsValid(MainCharacter->GetPawnNoiseEmitterComponent())) return;
	
	MainCharacter->GetPawnNoiseEmitterComponent()->MakeNoise(
		this, 1.0f, GetActorLocation());

	if (IsValid(FireSound)) UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void AWeapon::ResetShooting()
{
	bCanShoot = true;
}
void AWeapon::AssignToPlayer()
{
	AMainCharacter* Character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Character->SetEquippedWeapon(this);
}

