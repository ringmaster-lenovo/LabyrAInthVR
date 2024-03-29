#include "BaseEnemy.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LabyrAInthVR/Scene/Config.h"
#include "LabyrAInthVR/Scene/LabyrinthParser.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Enemy_Log);

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	WeaponBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBoxComponent"));
	WeaponBoxComponent->SetupAttachment(GetMesh(), FName("weapon_r"));
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultController();

	AIController = Cast<AAIController>(GetController());

	if (!IsValid(AIController)) return;

	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ThisClass::OnMoveFinished);

	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);

	if (!IsValid(WeaponBoxComponent)) return;

	WeaponBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ABaseEnemy::OnComponentBeginOverlap);

	if (!IsValid(PawnSensingComponent)) return;

	PawnSensingComponent->Activate();
	PawnSensingComponent->SetSensingUpdatesEnabled(true);
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ThisClass::OnSeePawn);
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &ThisClass::OnHearNoise);

	UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(this);

	if (!IsValid(NavigationSystemV1)) return;

	NavigationSystemV1->OnNavigationGenerationFinishedDelegate.AddDynamic(
		this, &ThisClass::NavMeshFinishedBuilding);
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (EnemyState)
	{
	case Ees_WaitingForNav:
		break;
	case Ees_Idle:
		StartPatrolling();
		break;
	case Ees_Patrolling:
		break;
	case Ees_Attacking:
		Attack();
		break;
	case Ees_Chasing:
		CheckAttack();
		break;
	case Ees_Hold:
		HoldPosition();
		break;
	case Ees_Dead:
		break;
	default: ;
	}
}

void ABaseEnemy::SetMatrixPosition(uint8 Row, uint8 Column)
{
	MatrixRow = Row;
	MatrixColumn = Column;
}

float ABaseEnemy::GetSpeed() const
{
	if (!IsValid(GetCharacterMovement())) return 0.f;

	return GetCharacterMovement()->Velocity.Size();
}

void ABaseEnemy::NavMeshFinishedBuilding(ANavigationData* NavigationData)
{
	EnemyState = Ees_Idle;
}

void ABaseEnemy::PatrollingTimerFinished()
{
	LabyrinthParser = LabyrinthParser == nullptr ? Cast<ALabyrinthParser>(GetOwner()) : LabyrinthParser;

	if (EnemyState == Ees_Attacking || !IsValid(LabyrinthParser) || !IsValid(AIController)) return;

	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Patrolling timer finished"))

	EnemyState = Ees_Patrolling;

	End = LabyrinthParser->GetNextDestination(MatrixRow, MatrixColumn, LastKnownDirection);
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(End);
	MoveRequest.SetAcceptanceRadius(0.f);
	AIController->MoveTo(MoveRequest);
}

void ABaseEnemy::AttackingTimerFinished()
{
	bCanAttack = true;
}

void ABaseEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                         const FHitResult& SweepResult)
{
	const ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (!IsValid(HitCharacter) || OtherActor == this) return;
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Enemy attacked: %s"), *OtherActor->GetName())
}

void ABaseEnemy::OnSeePawn(APawn* Pawn)
{
	if (EnemyState > Ees_Patrolling) return;

	SeenCharacter = SeenCharacter == nullptr ? Cast<ACharacter>(Pawn) : SeenCharacter;

	Chase();
}

void ABaseEnemy::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	// Only go to the location if we were roaming
	if (EnemyState > Ees_Patrolling || !IsValid(AIController)) return;

	AIController->StopMovement();
	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);

	EnemyState = Ees_Patrolling;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(0.f);
	AIController->MoveTo(MoveRequest);
}

void ABaseEnemy::OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult)
{
	// For some reason this always gets triggered so I have to clear the timer
	if (EnemyState != Ees_Patrolling)
	{
		GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
		return;
	}

	EnemyState = Ees_Idle;
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Move finished"));
}

// Whenever we chased we have to update the matrix position and we do it based on the X Y coordinates
void ABaseEnemy::UpdateMatrixPosition()
{
	LastKnownDirection = Eed_None;
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Previous position: %d %d"), MatrixRow, MatrixColumn);
	const float XPos = GetActorLocation().X;
	const float YPos = GetActorLocation().Y;

	MatrixColumn = XPos / WallSettings::WallOffset;
	MatrixRow = YPos / WallSettings::WallOffset;

	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("X Position: %f - Modulus: %f - Index: %d"), XPos,
	       FMath::Fmod(XPos, WallSettings::WallOffset), MatrixColumn);
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Y Position: %f - Modulus: %f - Index: %d"), YPos,
	       FMath::Fmod(YPos, WallSettings::WallOffset), MatrixRow);

	if (FMath::Fmod(XPos, WallSettings::WallOffset) > WallSettings::WallOffset / 2)
	{
		MatrixColumn++;
	}

	if (FMath::Fmod(YPos, WallSettings::WallOffset) > WallSettings::WallOffset / 2)
	{
		MatrixRow++;
	}

	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("New position: %d %d"), MatrixRow, MatrixColumn);

	const FVector NewPos{MatrixColumn * WallSettings::WallOffset, MatrixRow * WallSettings::WallOffset, 0.f};
	DrawDebugSphere(GetWorld(), NewPos, 20.f, 15, FColor::Green, true);
}

void ABaseEnemy::StartPatrolling()
{
	if (EnemyState != Ees_Idle || GetWorldTimerManager().TimerExists(PatrollingTimerHandle) || AIController == nullptr)
		return;

	const float RandomWaitTime = FMath::FRandRange(MinPatrolTimer, MaxPatrolTimer);
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Random wait time: %f"), RandomWaitTime);
	GetWorldTimerManager().SetTimer(PatrollingTimerHandle, this, &ThisClass::PatrollingTimerFinished, RandomWaitTime,
	                                false);
}

void ABaseEnemy::Chase()
{
	if (!IsValid(AIController) || !IsValid(SeenCharacter)) return;

	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
	EnemyState = Ees_Chasing;
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Initiating chase action to: %s"), *SeenCharacter->GetName());
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(SeenCharacter);
	MoveRequest.SetAcceptanceRadius(0.f);
	AIController->MoveTo(MoveRequest);
}

void ABaseEnemy::Attack()
{
	if (!IsValid(AIController)) return;

	RotateToCharacter();

	// If distance is greater than attack distance, it means we go back chasing
	if (GetDistanceToCharacter() > AttackDistance && !IsAttacking())
	{
		Chase();
	}

	if (bCanAttack)
	{
		bCanAttack = false;
		PlayMontage(AttackMontage);
		GetWorldTimerManager().SetTimer(AttackingTimerHandle, this, &ThisClass::AttackingTimerFinished, AttackTimer,
		                                false);
	}
}

void ABaseEnemy::HoldPosition()
{
	if (!IsValid(AIController) || !IsValid(SeenCharacter)) return;

	// If lost sight of character, go back to idle and patrolling
	if (!AIController->LineOfSightTo(SeenCharacter))
	{
		UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Lost sight of character, going back to idle"))
		UpdateMatrixPosition();
		EnemyState = Ees_Idle;
	}
	else if (GetDistanceToCharacter() <= ChaseDistance)
	{
		UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Character regained eligible distance for chasing, going back to chase"))
		Chase();
	}
	else if (GetDistanceToCharacter() > ChaseDistance)
	{
		// If distance greater than chase distance we stop if we're facing the character, otherwise we chase
		if (IsFacing()) AIController->StopMovement();
		else Chase();
	}
}

void ABaseEnemy::CheckAttack()
{
	// Stop chasing if distance greater than ChasingDistance
	if (GetDistanceToCharacter() > ChaseDistance && IsFacing())
	{
		UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Entering into hold phase"))
		AIController->StopMovement();
		EnemyState = Ees_Hold;
	}

	// Initiate attack phase if closer to character than attack distance
	if (GetDistanceToCharacter() < AttackDistance)
	{
		UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Entering into attacking phase"))
		AIController->StopMovement();
		bCanAttack = true;
		EnemyState = Ees_Attacking;
	}
}

void ABaseEnemy::RotateToCharacter()
{
	if (!IsValid(SeenCharacter)) return;

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),
	                                                                 SeenCharacter->GetActorLocation());
	TargetRotation.Roll = 0;
	TargetRotation.Pitch = 0;
	SetActorRotation(TargetRotation);
}

float ABaseEnemy::GetDistanceToCharacter()
{
	if (!IsValid(SeenCharacter)) return 0.f;
	return FVector::Distance(SeenCharacter->GetActorLocation(), GetActorLocation());
}

bool ABaseEnemy::IsAttacking()
{
	const UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance) || !IsValid(AttackMontage)) return false;

	return AnimInstance->Montage_IsPlaying(AttackMontage);
}

bool ABaseEnemy::IsFacing()
{
	if (!IsValid(SeenCharacter)) return false;
	
	return UKismetMathLibrary::DegAcos(
			FVector::DotProduct(SeenCharacter->GetActorForwardVector(), GetActorForwardVector())) >
		Misc::FacingEnemyDegrees;
}

void ABaseEnemy::PlayMontage(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance) || !IsValid(MontageToPlay)) return;
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Playing montage"))
	const FName SectionName{*FString::Printf(TEXT("%d"), FMath::RandRange(1, MontageToPlay->GetNumSections()))};
	AnimInstance->Montage_Play(MontageToPlay);
	AnimInstance->Montage_JumpToSection(SectionName);
}

void ABaseEnemy::SetWeaponCollision(bool bEnabled)
{
	if (!IsValid(WeaponBoxComponent)) return;

	WeaponBoxComponent->SetCollisionEnabled(bEnabled
		                                        ? ECollisionEnabled::QueryAndPhysics
		                                        : ECollisionEnabled::NoCollision);
}

void ABaseEnemy::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                               AController* InstigatedBy, AActor* DamageCauser)
{
	if (EnemyState == Ees_Dead) return;
	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("Received damage"))
	Health -= Damage;

	if (BloodEffect == nullptr) return;
	const FVector EffectSpawn {
		GetActorLocation() + FVector{0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.75, 0.f}
	};
	UGameplayStatics::SpawnEmitterAtLocation(this, BloodEffect, EffectSpawn);

	if (Health > 0) return;

	GetWorldTimerManager().ClearTimer(AttackingTimerHandle);
	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AIController->StopMovement();
	SetWeaponCollision(false);
	EnemyState = Ees_Dead;
	PlayMontage(DeathMontage);
}
