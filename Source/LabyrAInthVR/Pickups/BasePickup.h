#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "BasePickup.generated.h"

class AWeapon;
class USphereComponent;

UCLASS()
class LABYRAINTHVR_API ABasePickup : public AActor, public ISpawnableActor
{
	GENERATED_BODY()
	
public:	
	ABasePickup();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	FORCEINLINE TSubclassOf<AWeapon> GetWeapon() { return Weapon; }
	FORCEINLINE AWeapon* GetSpawnedWeapon() { return SpawnedWeapon; }
	FORCEINLINE void SetSpawnedWeapon(AWeapon* WeaponSpawned) { SpawnedWeapon = WeaponSpawned; }
	FORCEINLINE bool IsWeaponSpawned() { return SpawnedWeapon != nullptr; }
	
private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> Weapon;

	UPROPERTY()
	AWeapon* SpawnedWeapon;
};
