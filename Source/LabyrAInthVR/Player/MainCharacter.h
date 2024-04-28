// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "LabyrAInthVR/Interagibles/Weapon.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "MainCharacter.generated.h"

class UInputAction;
class USpotLightComponent;
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
	virtual void ResetWeapon();
	float GetWeaponDamage();
	bool IsAlive();
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							   AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	UPlayerStatistics* GetPlayerStatistics();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString PlayerName;
	
	UPROPERTY()
	ABasePickup* OverlappingPickup;
	
	UPROPERTY()
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UPlayerStatistics* PlayerStats;

	UPROPERTY(EditAnywhere, Category=Gameplay)
	USpotLightComponent* Flashlight;
	
	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* FlashlightInputAction;
	
	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* SprintInputAction;
	
	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* ShootInputAction;
	
	void ToggleFlashlight(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value, bool bSprint);
	void Shoot(const FInputActionValue& Value);
	void ReleasePickupObject();

	bool bHasWeapon;
	
	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void PickupWeapon();*/
public:
	FORCEINLINE void SetOverlappedPickup(ABasePickup* Pickup) {OverlappingPickup = Pickup; }
};