#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickup.generated.h"

class USphereComponent;

UCLASS()
class LABYRAINTHVR_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePickup();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	
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
};
