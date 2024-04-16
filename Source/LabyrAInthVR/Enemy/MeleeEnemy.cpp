#include "MeleeEnemy.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/MockedCharacter/MockedCharacter.h"
#include "LabyrAInthVR/Player/MainCharacter.h"

AMeleeEnemy::AMeleeEnemy()
{
	LeftWeaponBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBoxComponent"));
	LeftWeaponBoxComponent->SetupAttachment(GetMesh(), FName("weapon_l"));
	
	RightWeaponBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponBoxComponent"));
	RightWeaponBoxComponent->SetupAttachment(GetMesh(), FName("weapon_r"));
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(LeftWeaponBoxComponent)) return;

	LeftWeaponBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	RightWeaponBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	if(bHasDoubleWeapon) return;

	LeftWeaponBoxComponent->UnregisterComponent();
}

void AMeleeEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UE_LOG(LogTemp, Warning, TEXT("Enemy state: %s"), *UEnum::GetValueAsString(EnemyState))
	
	switch (EnemyState)
	{
	case EES_WaitingForNav:
		break;
	case EES_Idle:
		StartPatrolling();
		break;
	case EES_Patrolling:
		break;
	case EES_Attacking:
		Attack();
		break;
	case EES_Chasing:
		CheckDistances();
		break;
	case EES_Hold:
		HoldPosition();
		break;
	case EES_Dead:
		break;
	default: ;
	}

	//DrawDebugSphere(GetWorld(), GetActorLocation(), MeleeAttackDistance, 10, FColor::Red, false);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), ChaseDistance, 10, FColor::Turquoise, false);
}

void AMeleeEnemy::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                AController* InstigatedBy, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	if(Health > 0) return;
	
	if(bHasDoubleWeapon)
	{
		SetWeaponCollision(false, true);
		SetWeaponCollision(false, false);
	} else
	{
		SetWeaponCollision(false, true);
	}
	
	GetWorldTimerManager().ClearTimer(MeleeAttackTimerHandle);
}

void AMeleeEnemy::PlayMontage(UAnimMontage* MontageToPlay)
{
	Super::PlayMontage(MontageToPlay);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (MontageToPlay == MeleeAttackMontage) AnimInstance->Montage_SetPlayRate(MeleeAttackMontage, MeleeAttackSpeed);
}

void AMeleeEnemy::SetWeaponCollision(bool bEnabled, bool bRightWeapon)
{
	if(bRightWeapon)
	{
		if (!IsValid(RightWeaponBoxComponent)) return;

		RightWeaponBoxComponent->SetCollisionEnabled(bEnabled
													? ECollisionEnabled::QueryAndPhysics
													: ECollisionEnabled::NoCollision);
	} else
	{
		if (!IsValid(LeftWeaponBoxComponent)) return;

		LeftWeaponBoxComponent->SetCollisionEnabled(bEnabled
													? ECollisionEnabled::QueryAndPhysics
													: ECollisionEnabled::NoCollision);
	}
}

void AMeleeEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	const bool bShouldDamage = Cast<AMainCharacter>(OtherActor) == nullptr || Cast<ABaseEnemy>(OtherActor) != nullptr;
	
	if (bShouldDamage || OtherActor == this || !OtherActor->Implements<UDamageableActor>()) return;
	UE_LOG(LogTemp, Warning, TEXT("Collision with: %s"), *OtherActor->GetName())
	UGameplayStatics::ApplyDamage(OtherActor, MeleeAttackDamage, GetController(), this, UDamageType::StaticClass());
	StatsChangerComponent->ChangeStats(Stats);
}

void AMeleeEnemy::Attack()
{
	if (!IsValid(AIController) || !IsValid(NavigationSystemV1) || !CanExecuteAction()) return;

	RotateToCharacter();

	AttackInternal();
}

void AMeleeEnemy::AttackInternal()
{
	// If distance is greater than melee attack distance, it means we go back chasing
	if (GetDistanceToCharacter() > MeleeAttackDistance && !IsAttacking())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetDistanceToCharacter() > MeleeAttackDistance true so chase"))
		Chase();
		return;
	}
	
	if (!bCanAttack) return;

	bCanAttack = false;
	PlayMontage(MeleeAttackMontage);
	GetWorldTimerManager().SetTimer(MeleeAttackTimerHandle, this, &ThisClass::AttackingTimerFinished, MeleeAttackTimer,
	                                false);
}

void AMeleeEnemy::AttackingTimerFinished()
{
	bCanAttack = true;
}

bool AMeleeEnemy::CanExecuteAction()
{
	if (!IsValid(AIController)) return false;
	if (!IsValid(SeenCharacter) || !IsCharacterOnNavMesh() || !SeenCharacter->IsAlive())
	{
		GetWorldTimerManager().ClearTimer(MeleeAttackTimerHandle);
		AIController->StopMovement();
		UpdateMatrixPosition();
		UE_LOG(LogTemp, Warning, TEXT("Execute action failed"))
		EnemyState = EES_Idle;
		return false;
	}
	return true;
}

void AMeleeEnemy::HoldPosition()
{
	if (!IsValid(AIController) || !CanExecuteAction()) return;

	// If lost sight of character, go back to idle and patrolling
	if (!AIController->LineOfSightTo(SeenCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Lost sight of character, going back to idle"))
		UpdateMatrixPosition();
		EnemyState = EES_Idle;
	}
	else if (GetDistanceToCharacter() <= ChaseDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character regained eligible distance for chasing, going back to chase"))
		Chase();
	}
	else if (GetDistanceToCharacter() > ChaseDistance)
	{
		// If distance greater than chase distance we stop if we're facing the character, otherwise we chase
		if (IsFacing()) AIController->StopMovement();
		else Chase();
	}
}

void AMeleeEnemy::CheckDistances()
{
	if (!CanExecuteAction()) return;

	// Stop chasing if distance greater than ChasingDistance
	if (GetDistanceToCharacter() >= ChaseDistance && IsFacing())
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering into hold phase"))
		AIController->StopMovement();
		EnemyState = EES_Hold;
	}

	// Initiate attack phase if closer to character than attack distance
	if (GetDistanceToCharacter() <= MeleeAttackDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering into melee attacking phase"))
		AIController->StopMovement();
		bCanAttack = true;
		EnemyState = EES_Attacking;
	}
}

float AMeleeEnemy::GetDistanceToCharacter()
{
	if (!IsValid(SeenCharacter)) return 0.f;
	return FVector::Distance(SeenCharacter->GetActorLocation(), GetActorLocation());
}

bool AMeleeEnemy::IsAttacking()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance) || !IsValid(MeleeAttackMontage)) return false;

	return AnimInstance->Montage_IsPlaying(MeleeAttackMontage);
}
