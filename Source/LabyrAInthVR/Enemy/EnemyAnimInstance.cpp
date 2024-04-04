#include "EnemyAnimInstance.h"

#include "BaseEnemy.h"
#include "MeleeEnemy.h"

void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	BaseEnemy = Cast<ABaseEnemy>(TryGetPawnOwner());
	MeleeEnemy = Cast<AMeleeEnemy>(BaseEnemy);
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!IsValid(BaseEnemy)) return;

	Speed = BaseEnemy->GetSpeed();
}
