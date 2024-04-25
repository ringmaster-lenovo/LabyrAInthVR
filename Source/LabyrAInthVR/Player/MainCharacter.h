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
	void StartLevelTimer();

	UFUNCTION(BlueprintCallable)
	UPlayerStatistics* GetPlayerStatistics();
protected:
	void StopAllTimers();

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& Name) { PlayerName = Name; }

	UFUNCTION(BlueprintCallable)
	bool IsShieldActive() const { return Shield; }

	UFUNCTION(BlueprintCallable)
	int32 GetTimeOnCurrentLevel() const { return TimeOnCurrentLevel; }

	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() const { return PlayerName; }
	
protected:
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	// AWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int CurrentHours = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int CurrentMinutes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int CurrentSeconds = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int32 TimeOnCurrentLevel = 0;

	int32 SpeedTimer = 0;

	int32 SpeedTimerGoesOff = 0;

	FTimerHandle TimerOnLevelHandle;

	FTimerHandle SpeedTimerHandle;

	UFUNCTION(BlueprintCallable)
	void UpdateTimeOnCurrentLevel();

	void UpdateSpeedTimer();
	
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
