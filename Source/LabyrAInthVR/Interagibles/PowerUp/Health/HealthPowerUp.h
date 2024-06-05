#pragma once
#include "LabyrAInthVR/Interagibles/PowerUp/BasePowerUp.h"
#include "HealthPowerUp.generated.h"

UCLASS()
class LABYRAINTHVR_API AHealthPowerUp : public ABasePowerUp
{
	GENERATED_BODY()
public:
	AHealthPowerUp();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PowerUp logic
	virtual void PowerUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
