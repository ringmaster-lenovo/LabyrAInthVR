#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "LabyrAInthVR/Interfaces/MovableActor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "LabyrAInthVR/Scene/SpawnManager.h"
#include "BasePowerUp.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_PowerUp_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API ABasePowerUp : public AActor, public ISpawnableActor, public IMovableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePowerUp();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PowerUp type
	UPROPERTY(EditAnywhere)
	EPowerUpsTypes PowerUpType;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Called when the powerup collide
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);
	// PowerUp the player
	UFUNCTION()
	void PowerUp(AMainCharacter* MainCharacter);
private:
	/** Sound to play on collect */
	UPROPERTY(EditAnywhere)
	USoundBase* CollectSound;
	
	/** Collision component */
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	//Object component
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* NiagaraComponent;

	//Emitter
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraEmitter;
};
