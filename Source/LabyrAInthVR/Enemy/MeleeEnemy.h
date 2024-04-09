#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "MeleeEnemy.generated.h"

UCLASS()
class LABYRAINTHVR_API AMeleeEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMeleeEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                           AController* InstigatedBy, AActor* DamageCauser) override;
	virtual void PlayMontage(UAnimMontage* MontageToPlay) override;
	virtual void AttackInternal();
	virtual void CheckDistances();
	float GetDistanceToCharacter();
	bool IsAttacking();

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float MeleeAttackDistance{200.f};
	
	UFUNCTION()
	void AttackingTimerFinished();
	
	bool bCanAttack{true};

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* MeleeAttackMontage;

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float MeleeAttackTimer{5.f};

	FTimerHandle MeleeAttackTimerHandle;

private:
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(bool bEnabled, bool bRightWeapon);

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);

	bool CanExecuteAction();
	void HoldPosition();
	void Attack();

private:
	UPROPERTY(EditAnywhere, Category=Weapon)
	UBoxComponent* LeftWeaponBoxComponent;

	UPROPERTY(EditAnywhere, Category=Weapon)
	UBoxComponent* RightWeaponBoxComponent;

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float MeleeAttackDamage{50.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	bool bHasDoubleWeapon{false};
	
	UPROPERTY(EditAnywhere, Category=Settings)
	float ChaseDistance{2000.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float MeleeAttackSpeed{1.f};
};
