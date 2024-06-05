#pragma once

#include "LabyrAInthVR/Interagibles/PowerUp/BasePowerUp.h"
#include "CompassPowerUp.generated.h"

UCLASS()
class LABYRAINTHVR_API ACompassPowerUp : public ABasePowerUp
{
	GENERATED_BODY()
public:
	ACompassPowerUp();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PowerUp logic
	virtual void PowerUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* CompassEffect;
};
