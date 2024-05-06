#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FreezableActor.generated.h"

UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UFreezableActor : public UInterface
{
	GENERATED_BODY()
};

class LABYRAINTHVR_API IFreezableActor
{
	GENERATED_BODY()

public:
	virtual void Freeze(int32 Time);
	virtual void Unfreeze();
};
