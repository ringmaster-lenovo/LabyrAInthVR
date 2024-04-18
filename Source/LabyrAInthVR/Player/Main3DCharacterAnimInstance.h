#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Main3DCharacterAnimInstance.generated.h"

class AMain3DCharacter;

UCLASS()
class LABYRAINTHVR_API UMain3DCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	AMain3DCharacter* MainCharacter;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsMoving;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bHasWeapon;
};
