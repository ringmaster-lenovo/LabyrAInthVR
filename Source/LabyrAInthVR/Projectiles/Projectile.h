#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;

UCLASS()
class LABYRAINTHVR_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AProjectile();
	virtual void BeginPlay() override;

private:
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
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	
	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ProjectileTracer;

	UPROPERTY()
	UParticleSystemComponent* ProjectileTracerComponent;
};
