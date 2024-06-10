#include "FrozenStarPowerUp.h"

#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/VRGameMode.h"
#include "LabyrAInthVR/Interagibles/PowerUp/Utils/Properties.h"

AFrozenStarPowerUp::AFrozenStarPowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFrozenStarPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFrozenStarPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFrozenStarPowerUp::PowerUp()
{
	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!IsValid(GameMode) || !IsValid(GameMode->GetSceneController()))
	{
		return;
	}

	GameMode->GetSceneController()->TriggerFrozenStar(FrozenStarProperties::PauseDuration);
}
