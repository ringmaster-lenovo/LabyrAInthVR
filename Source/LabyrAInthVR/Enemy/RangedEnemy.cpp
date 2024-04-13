#include "RangedEnemy.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/MockedCharacter/MockedCharacter.h"
#include "LabyrAInthVR/Projectiles/Projectile.h"

void ARangedEnemy::AttackInternal()
{
	// If distance is greater than ranged attack distance, it means we go back chasing
	if (GetDistanceToCharacter() > RangedAttackDistance && !IsAttacking())
	{
		Chase();
		return;
	}

	// If distance is between ranged and melee then start shooting mode
	bShootingMode = GetDistanceToCharacter() <= RangedAttackDistance && GetDistanceToCharacter() > MeleeAttackDistance;

	if (!bCanAttack) return;

	bCanAttack = false;
	PlayMontage(bShootingMode ? RangedAttackMontage : MeleeAttackMontage);
	GetWorldTimerManager().SetTimer(bShootingMode ? RangedAttackTimerHandle : MeleeAttackTimerHandle, this,
	                                &ThisClass::AttackingTimerFinished, bShootingMode ? RangedAttackTimer : MeleeAttackTimer,
	                                false);
}

void ARangedEnemy::CheckDistances()
{
	Super::CheckDistances();
	
	if (GetDistanceToCharacter() <= RangedAttackDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering into ranged attacking phase"))
		AIController->StopMovement();
		bCanAttack = true;
		EnemyState = EES_Attacking;
	}
}

void ARangedEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DrawDebugSphere(GetWorld(), GetActorLocation(), RangedAttackDistance, 10, FColor::Green, false);
}

void ARangedEnemy::ShootProjectile()
{
	FVector SocketLocation = GetMesh()->GetSocketTransform(AmmoEjectSocketName).GetLocation();
	
	if(MuzzleEffectParticle == nullptr) return;
	
	UGameplayStatics::SpawnEmitterAtLocation(this, MuzzleEffectParticle, SocketLocation);
	
	if(ProjectileClass == nullptr || !IsValid(GetMesh())) return;
	
	FVector EndVector = SeenCharacter->GetActorLocation() - SocketLocation;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.Owner = this;
	
	GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SocketLocation, EndVector.Rotation(), SpawnParameters);
}
