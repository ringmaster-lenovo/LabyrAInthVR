#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interfaces/MovableActor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "LabyrAInthVR/Projectiles/Projectile.h"
#include "Weapon.generated.h"

class AMainCharacter;
class ABasePickup;
class UStatsChangerComponent;
class AMainCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Weapon_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API AWeapon : public AActor, public ISpawnableActor, public IMovableActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void Shoot();
	void Destroyed() override;
protected:
	virtual void BeginPlay() override;

private:
	void ShootInternal();
	void ResetShooting();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double Damage = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double FireRate {0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	uint8 BulletsPerBurst {4};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;
	
	UPROPERTY()
	ABasePickup* Pickup;
		
	UPROPERTY()
	bool bHasBeenFound = false;
	
	UPROPERTY(EditAnywhere)
	bool bShouldDelayInput{true};
	
	UPROPERTY()
	AMainCharacter* MainCharacter;

	FTimerHandle ShootingTimerHandle;
	
	bool bCanShoot{true};
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USoundBase* FireSound;
	
public:
	FORCEINLINE float GetDamage() {return Damage; }
	FORCEINLINE float GetFirerate() {return FireRate; }
	FORCEINLINE TSubclassOf<AProjectile> GetProjectileTemplate() {return ProjectileClass; }
	FORCEINLINE UParticleSystem* GetMuzzleEffect() {return MuzzleEffect; }
	FORCEINLINE UAnimationAsset* GetAnimation() { return FireAnimation; }
	FORCEINLINE void SetPickup(ABasePickup* BasePickup) { Pickup = BasePickup; }
	FORCEINLINE ABasePickup* GetPickup() {return Pickup; }
	FORCEINLINE void SetMainCharacter(AMainCharacter* CharacterMain) { MainCharacter = CharacterMain; }

	UFUNCTION(BlueprintCallable)
	void AssignToPlayerRight();

	UFUNCTION(BlueprintCallable)
	void RemoveFromPlayerRight();

	UFUNCTION(BlueprintCallable)
	void AssignToPlayerLeft();

	UFUNCTION(BlueprintCallable)
	void RemoveFromPlayerLeft();
	
	UFUNCTION(BlueprintCallable)
	bool HasBeenFound() const { return bHasBeenFound; }

	UFUNCTION(BlueprintCallable)
	void SetHasBeenFound() { bHasBeenFound = true; }
};
