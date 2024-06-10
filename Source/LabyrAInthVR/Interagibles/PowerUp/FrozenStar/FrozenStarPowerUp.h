#pragma once
#include "LabyrAInthVR/Interagibles/PowerUp/BasePowerUp.h"
#include "FrozenStarPowerUp.generated.h"

UCLASS()
class AFrozenStarPowerUp: public ABasePowerUp
{
	GENERATED_BODY()
public:
	AFrozenStarPowerUp();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PowerUp logic
	virtual void PowerUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
