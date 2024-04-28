#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class AProceduralSplineWall;
class UBoxComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Projectiles_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AProjectile();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	void SetDamage(double DamagePassed) {Damage = DamagePassed;}
	
private:
	double Damage = 20;
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* ProjectileMesh;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ProjectileTracer;

	UPROPERTY()
	UParticleSystemComponent* ProjectileTracerComponent;
};
