// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LabyrAInthVR/Interagibles/Weapon.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "MainCharacter.generated.h"

class UPlayerStatistics;
class ABasePickup;
DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Character_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API AMainCharacter : public ACharacter, public IDamageableActor
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	bool IsAlive();
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							   AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable)
	void StartTimer();

	UFUNCTION(BlueprintCallable)
	UPlayerStatistics* GetPlayerStatistics();
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponClass;
	
	UPROPERTY()
	ABasePickup* OverlappingPickup;
	
	UPROPERTY()
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UPlayerStatistics* PlayerStats;

	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void PickupWeapon();*/
public:
	FORCEINLINE void SetOverlappedPickup(ABasePickup* Pickup) {OverlappingPickup = Pickup; }
};
