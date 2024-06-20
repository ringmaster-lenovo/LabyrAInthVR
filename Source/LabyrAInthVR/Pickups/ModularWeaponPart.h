#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Player/ModularWeapon.h"
#include "ModularWeaponPart.generated.h"

class USphereComponent;

UCLASS()
class LABYRAINTHVR_API AModularWeaponPart : public AActor
{
	GENERATED_BODY()
	
public:	
	AModularWeaponPart();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)	
	USkeletalMesh* ModularPartSkeletalMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ModularPartStaticMesh;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EWeaponPart> WeaponPart;

public:
	USkeletalMesh* GetSkeletalMeshPart() { return ModularPartSkeletalMesh; }
	EWeaponPart GetWeaponPart() { return WeaponPart; }
};
