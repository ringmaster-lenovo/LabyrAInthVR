#include "HealthPowerUp.h"
#include "LabyrAInthVR/Interagibles/PowerUp/Utils/PlayerStatsVisitor.h"
#include "LabyrAInthVR/Interagibles/PowerUp/Utils/Properties.h"

AHealthPowerUp::AHealthPowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHealthPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHealthPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealthPowerUp::PowerUp()
{
	if(IsValid(MainCharacter))
	{
		UPlayerStatsVisitor::ModifyHealth(MainCharacter, HealthProperties::BonusHealth);
	}
}
