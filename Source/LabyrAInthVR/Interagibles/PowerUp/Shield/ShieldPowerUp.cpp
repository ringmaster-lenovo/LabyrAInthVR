#include "ShieldPowerUp.h"
#include "LabyrAInthVR/Interagibles/PowerUp/Utils/PlayerStatsVisitor.h"

AShieldPowerUp::AShieldPowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShieldPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShieldPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShieldPowerUp::PowerUp()
{
	if(IsValid(MainCharacter))
	{
		UPlayerStatsVisitor::ModifyShield(MainCharacter, true);
	}
}