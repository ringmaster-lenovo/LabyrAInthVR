#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "EffectManager.generated.h"

enum class EPowerUpsTypes : uint8;

/**
 * EffectManager class
 * Contains utility functions to apply PowerUp logic
 */
UCLASS()
class LABYRAINTHVR_API UEffectManager: public UObject
{
public:

	GENERATED_BODY()
	
	//Apply PowerUp based on a specific PowerUpType
	static void ApplyEffect(EPowerUpsTypes PowerUpsType, AMainCharacter* MainCharacter);
	
};
