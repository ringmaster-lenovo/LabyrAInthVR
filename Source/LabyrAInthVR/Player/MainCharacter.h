// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "MainCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVR, Log, All);

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
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							   AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double MovementSpeed = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	bool Shield = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics", meta = (AllowPrivateAccess = "true"))
	double Life = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int CurrentHours = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int CurrentMinutes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	int CurrentSeconds = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	FString name;

	UPROPERTY()
	AActor* OverlappedPickup;

public:
	FORCEINLINE void SetOverlappedPickup(AActor* NewPickup) { OverlappedPickup = NewPickup; }
};
