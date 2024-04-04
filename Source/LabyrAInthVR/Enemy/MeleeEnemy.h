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

protected:
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;
private:
	UPROPERTY(EditAnywhere, Category=Weapon)
	UBoxComponent* WeaponBoxComponent;

private:
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(bool bEnabled);
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);
};
