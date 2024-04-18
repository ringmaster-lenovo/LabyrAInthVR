// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Projectiles/Projectile.h"
#include "Weapon.generated.h"

class ABasePickup;
class UStatsChangerComponent;

UCLASS()
class LABYRAINTHVR_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE TSubclassOf<AProjectile> GetProjectileTemplate() {return ProjectileClass; }
	FORCEINLINE UParticleSystem* GetMuzzleEffect() {return MuzzleEffect; }
	FORCEINLINE UAnimationAsset* GetAnimation() { return FireAnimation; }
	FORCEINLINE void SetPickup(ABasePickup* BasePickup) { Pickup = BasePickup; }
	FORCEINLINE ABasePickup* GetPickup() {return Pickup; }
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double Damage = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	bool IsAutomatic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double FireRate;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* FireAnimation;

	UPROPERTY()
	ABasePickup* Pickup;
};
