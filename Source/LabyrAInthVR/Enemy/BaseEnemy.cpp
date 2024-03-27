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
		CheckAttack();
		break;
	case EES_Hold:
		HoldPosition();
		break;
	case EES_Dead:
		break;
	default: ;
	}
}

void ABaseEnemy::SetMatrixPosition(uint8 Row, uint8 Column)
{
	MatrixRow = Row;
	MatrixColumn = Column;
}

float ABaseEnemy::GetSpeed()
{
	if (!IsValid(GetCharacterMovement())) return 0.f;

	return GetCharacterMovement()->Velocity.Size();
}

void ABaseEnemy::NavMeshFinishedBuilding(ANavigationData* NavigationData)
{
	EnemyState = EES_Idle;
}

void ABaseEnemy::PatrollingTimerFinished()
{
	LabyrinthParser = LabyrinthParser == nullptr ? Cast<ALabyrinthParser>(GetOwner()) : LabyrinthParser;

	if (EnemyState == EES_Attacking || !IsValid(LabyrinthParser) || !IsValid(AIController)) return;

	UE_LOG(LogTemp, Warning, TEXT("Patrolling timer finished"))

	EnemyState = EES_Patrolling;

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
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (!IsValid(HitCharacter) || OtherActor == this) return;
	UE_LOG(LogTemp, Warning, TEXT("Enemy attacked: %s"), *OtherActor->GetName())
}

void ABaseEnemy::OnSeePawn(APawn* Pawn)
{
	if (EnemyState > EES_Patrolling) return;

	SeenCharacter = SeenCharacter == nullptr ? Cast<ACharacter>(Pawn) : SeenCharacter;

	Chase();
}

void ABaseEnemy::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	// Only go to the location if we were roaming
	if (EnemyState > EES_Patrolling || !IsValid(AIController)) return;

	AIController->StopMovement();
	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);

	EnemyState = EES_Patrolling;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(0.f);
	AIController->MoveTo(MoveRequest);
}

void ABaseEnemy::OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult)
{
	// For some reason this always gets triggered so I have to clear the timer
	if (EnemyState != EES_Patrolling)
	{
		GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
		return;
	}

	EnemyState = EES_Idle;
	UE_LOG(LogTemp, Warning, TEXT("Move finished"));
}

// Whenever we chased we have to update the matrix position and we do it based on the X Y coordinates
void ABaseEnemy::UpdateMatrixPosition()
{
	LastKnownDirection = EED_None;
	UE_LOG(LogTemp, Warning, TEXT("Previous position: %d %d"), MatrixRow, MatrixColumn);
	float XPos = GetActorLocation().X;
	float YPos = GetActorLocation().Y;

	MatrixColumn = XPos / WallSettings::WallOffset;
	MatrixRow = YPos / WallSettings::WallOffset;

	UE_LOG(LogTemp, Warning, TEXT("X Position: %f - Modulus: %f - Index: %d"), XPos,
	       FMath::Fmod(XPos, WallSettings::WallOffset), MatrixColumn);
	UE_LOG(LogTemp, Warning, TEXT("Y Position: %f - Modulus: %f - Index: %d"), YPos,
	       FMath::Fmod(YPos, WallSettings::WallOffset), MatrixRow);

	if (FMath::Fmod(XPos, WallSettings::WallOffset) > WallSettings::WallOffset / 2)
	{
		MatrixColumn++;
	}

	if (FMath::Fmod(YPos, WallSettings::WallOffset) > WallSettings::WallOffset / 2)
	{
		MatrixRow++;
	}

	UE_LOG(LogTemp, Warning, TEXT("New position: %d %d"), MatrixRow, MatrixColumn);

	FVector NewPos{MatrixColumn * WallSettings::WallOffset, MatrixRow * WallSettings::WallOffset, 0.f};
	DrawDebugSphere(GetWorld(), NewPos, 20.f, 15, FColor::Green, true);
}

void ABaseEnemy::StartPatrolling()
{
	if (EnemyState != EES_Idle || GetWorldTimerManager().TimerExists(PatrollingTimerHandle) || AIController == nullptr)
		return;

	const float RandomWaitTime = FMath::FRandRange(MinPatrolTimer, MaxPatrolTimer);
	UE_LOG(LogTemp, Warning, TEXT("Random wait time: %f"), RandomWaitTime);
	GetWorldTimerManager().SetTimer(PatrollingTimerHandle, this, &ThisClass::PatrollingTimerFinished, RandomWaitTime,
	                                false);
}

void ABaseEnemy::Chase()
{
	if (!IsValid(AIController) || !IsValid(SeenCharacter)) return;

	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
	EnemyState = EES_Chasing;
	UE_LOG(LogTemp, Warning, TEXT("Initiating chase action to: %s"), *SeenCharacter->GetName());
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

void ABaseEnemy::CheckAttack()
{
	// Stop chasing if distance greater than ChasingDistance
	if (GetDistanceToCharacter() > ChaseDistance && IsFacing())
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering into hold phase"))
		AIController->StopMovement();
		EnemyState = EES_Hold;
	}

	// Initiate attack phase if closer to character than attack distance
	if (GetDistanceToCharacter() < AttackDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering into attacking phase"))
		AIController->StopMovement();
		bCanAttack = true;
		EnemyState = EES_Attacking;
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

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
	UE_LOG(LogTemp, Warning, TEXT("Playing montage"))
	FName SectionName{*FString::Printf(TEXT("%d"), FMath::RandRange(1, MontageToPlay->GetNumSections()))};
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
	if (EnemyState == EES_Dead) return;
	UE_LOG(LogTemp, Warning, TEXT("Received damage"))
	Health -= Damage;

	if (BloodEffect == nullptr) return;
	FVector EffectSpawn{
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
	EnemyState = EES_Dead;
	PlayMontage(DeathMontage);
}
