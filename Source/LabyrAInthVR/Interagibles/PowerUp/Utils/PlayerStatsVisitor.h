#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "PlayerStatsVisitor.generated.h"

/**
 * PlayerStatsVisitor class
 * Contains utility functions to interact with player stats
 */
UCLASS()
class LABYRAINTHVR_API UPlayerStatsVisitor: public UObject
{
public:

	GENERATED_BODY()

	//Add BonusHealth to player (subtract with negative BonusHealth)
	static void ModifyHealth(AMainCharacter* MainCharacter, float BonusHealth);

	//Active schield
	static void ModifyShield(AMainCharacter* MainCharacter, bool IsActive);
};
