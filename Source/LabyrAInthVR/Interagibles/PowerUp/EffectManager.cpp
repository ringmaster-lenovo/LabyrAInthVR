#include "EffectManager.h"
#include "LabyrAInthVR/Scene/SpawnManager.h"
#include "Utils/Properties.h"
#include "Utils/PlayerStatsVisitor.h"

void UEffectManager::ApplyEffect(EPowerUpsTypes PowerUpsType, AMainCharacter* MainCharacter)
{
	switch(PowerUpsType)
	{
	case EPowerUpsTypes::Ept_Heart:
		UPlayerStatsVisitor::ModifyHealth(MainCharacter, HealthProperties::BonusHealth);
		break;

	case EPowerUpsTypes::Ept_Lightning:
		break;

	case EPowerUpsTypes::Ept_Shield:
		UPlayerStatsVisitor::ModifyShield(MainCharacter, true);
		break;

	case EPowerUpsTypes::Ept_FrozenStar:
		break;

	case EPowerUpsTypes::Ept_Diamond:
		break;

	default:
		break;
	}
}
