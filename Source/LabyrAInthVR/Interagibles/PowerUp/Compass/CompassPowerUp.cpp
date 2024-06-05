#include "CompassPowerUp.h"

#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/VRGameMode.h"

ACompassPowerUp::ACompassPowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACompassPowerUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACompassPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACompassPowerUp::PowerUp()
{
	UCompassComponent* CompassComponent=MainCharacter->GetCompassComponent();
	if(IsValid(CompassComponent))
	{
		CompassComponent->Trigger(CompassEffect);
	}
}

