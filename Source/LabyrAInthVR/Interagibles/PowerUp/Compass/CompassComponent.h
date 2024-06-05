#pragma once

#include "CompassComponent.generated.h"

UCLASS()
class LABYRAINTHVR_API UCompassComponent: public UActorComponent
{
	GENERATED_BODY()

	UCompassComponent();
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	void Trigger(UParticleSystem* CompassEffect);

	void ClearCompassEffect();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UParticleSystemComponent* CompassInstance;
	
	FVector EndPortalPosition;
	FRotator EndPortalRotation;
};
