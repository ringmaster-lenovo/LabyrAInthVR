#include "Main3DCharacterAnimInstance.h"

#include "Main3DCharacter.h" 
#include "GameFramework/PawnMovementComponent.h"

void UMain3DCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if(!IsValid(TryGetPawnOwner())) return;
	MainCharacter = Cast<AMain3DCharacter>(TryGetPawnOwner());
}

void UMain3DCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!IsValid(TryGetPawnOwner())) return;

	bIsMoving = TryGetPawnOwner()->GetVelocity().Length() > 0;
	bIsFalling = TryGetPawnOwner()->GetMovementComponent()->IsFalling();

	if(!IsValid(MainCharacter)) return;
	
	bHasWeapon = MainCharacter->GetHasWeapon();
}
