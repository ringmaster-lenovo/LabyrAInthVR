// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LabyrAInthVR/Interagibles/Weapon.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "MainCharacter.generated.h"

class ABasePickup;

UCLASS()
class LABYRAINTHVR_API AMainCharacter : public ACharacter, public IDamageableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const { return Life > 0; }

	FORCEINLINE void ActivateShield() { Shield = true; }
	FORCEINLINE void DectivateShield() { Shield = false; }
	FORCEINLINE void SetOverlappedPickup(ABasePickup* Pickup) {OverlappingPickup = Pickup; }

	UFUNCTION()
	virtual void ReceiveDamage(float Damage, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	void StartLevelTimer();

	UFUNCTION(BlueprintCallable)
	void StopLevelTimer();

	UFUNCTION(BlueprintCallable)
	bool IsTimerActive() const;

	UFUNCTION(BlueprintCallable)
	void StopAllTimers();

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& Name) { PlayerName = Name; }

	UFUNCTION(BlueprintCallable)
	void ChangeSpeed(double SpeedIncrement, int32 Timer);

	UFUNCTION(BlueprintCallable)
	bool IsShieldActive() const { return Shield; }

	UFUNCTION(BlueprintCallable)
	int32 GetTimeOnCurrentLevel() const { return TimeOnCurrentLevel; }

	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
	void ResetStats();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double BaseSpeed = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double RunningSpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	bool Shield = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double Life = 100;

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
};
