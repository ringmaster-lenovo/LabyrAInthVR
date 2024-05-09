#pragma once

#include "CoreMinimal.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.generated.h"

class AProjectile;

UCLASS()
class LABYRAINTHVR_API ARangedEnemy : public AMeleeEnemy
{
	GENERATED_BODY()
protected:
	virtual void AttackInternal() override;
	virtual void CheckDistances() override;
	virtual void Tick(float DeltaSeconds) override;
private:
	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* RangedAttackMontage;

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float RangedAttackDistance{1000.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float ProjectilePathLength{1000.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float RangedAttackTimer{5.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float RangedAttackDamage{10.f};

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleEffectParticle;

	UPROPERTY(EditAnywhere, Category=Audio)
	USoundBase* RangedAttackSound;

	UPROPERTY(EditAnywhere)
	FName AmmoEjectSocketName;
	
	UFUNCTION(BlueprintCallable)
	void ShootProjectile();
	
	bool bShootingMode {true};
	FTimerHandle RangedAttackTimerHandle;
};
