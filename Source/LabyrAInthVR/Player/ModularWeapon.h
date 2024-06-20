#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interagibles/Weapon.h"
#include "ModularWeapon.generated.h"

class AModularWeaponPart;

UENUM(BlueprintType)
enum EWeaponPart : uint8
{
	Ewp_HandGuard UMETA(DisplayName = "Hand Guard"),
	Ewp_Stock UMETA(DisplayName = "Stock"),
	Ewp_Scope UMETA(DisplayName = "Scope"),
	Ewp_Ammo UMETA(DisplayName = "Ammo"),
	Ewp_Barrel UMETA(DisplayName = "Barrel"),
	Ewp_Magazine UMETA(DisplayName = "Magazine"),
	Ewp_Trigger UMETA(DisplayName = "Trigger"),
	Ewp_None UMETA(DisplayName = "None")
};

UCLASS()
class LABYRAINTHVR_API AModularWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	AModularWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void AttachModularPart(AModularWeaponPart* ModularWeaponPart);
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* HandGuard;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Stock;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Scope;
	
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Ammo;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Barrel;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Magazine;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Trigger;

	TMap<EWeaponPart, AModularWeaponPart*> ModularParts;
};
