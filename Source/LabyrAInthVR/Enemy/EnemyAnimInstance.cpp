#include "EnemyAnimInstance.h"

#include "BaseEnemy.h"

void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	BaseEnemy = Cast<ABaseEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BaseEnemy == nullptr) return;

	Speed = BaseEnemy->GetSpeed();
}
