#include "BaseEnemy.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LabyrAInthVR/Network/LabyrinthDTO.h"
#include "LabyrAInthVR/Scene/Config.h"
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
	if (EnemyState == EES_Attacking || !IsValid(AIController)) return;

	UE_LOG(LogTemp, Warning, TEXT("Patrolling timer finished"))

	EnemyState = EES_Patrolling;

	End = GetNextDestination(MatrixRow, MatrixColumn, LastKnownDirection);
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

FVector ABaseEnemy::GetNextDestination(uint8& Row, uint8& Column, EEnemyDirection& LastDirection)
{
	EEnemyDirection ChosenDirection{EED_None};
	TArray<EEnemyDirection> FreeEnemyDirections{};
	bool bIsInRoom{false};
	// Get all free roaming directions
	FillFreeDirections(Row, Column, FreeEnemyDirections);
	bIsInRoom = IsInRoom(Row, Column, FreeEnemyDirections);
	// Check if is in room
	if (bIsInRoom)
	{
		FreeEnemyDirections.Remove(EED_Diagonal);
		UE_LOG(LogTemp, Error, TEXT("The enemy is standing at the start of a room"))
	}

	// Pick direction based on the last known direction
	switch (LastDirection)
	{
	case EED_None:
		ChosenDirection = FreeEnemyDirections.Num() > 0
			                  ? FreeEnemyDirections[FMath::RandRange(0, FreeEnemyDirections.Num() - 1)]
			                  : EED_None;
		break;
	case EED_Left:
	case EED_Right:
		ChooseNextDirection(FreeEnemyDirections, ChosenDirection, LastDirection, 2, FreeEnemyDirections.Num() - 1);
		break;
	case EED_Down:
	case EED_Up:
		ChooseNextDirection(FreeEnemyDirections, ChosenDirection, LastDirection, 0, 1);
		break;
	case EED_Diagonal:
		ChooseNextDirection(FreeEnemyDirections, ChosenDirection, LastDirection, 0, FreeEnemyDirections.Num() - 1);
	default: ;
	}

	UE_LOG(LogTemp, Warning, TEXT("Chosen direction: %s"), *UEnum::GetValueAsString(ChosenDirection))

	// Find the (X,Y) for the chosen direction, stop at intersections
	uint8 TravellingRow{Row};
	uint8 TravellingColumn{Column};

	UE_LOG(LogTemp, Warning, TEXT("Current enemy indexes: %d %d"), TravellingRow, TravellingColumn)

	switch (ChosenDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		while (TravellingColumn - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[TravellingRow][TravellingColumn - 1] != 1 &&
			LabyrinthDTO->LabyrinthStructure[TravellingRow][TravellingColumn - 1] != 3)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			--TravellingColumn;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Right:
		while (TravellingColumn + 1 < std::size(LabyrinthDTO->LabyrinthStructure[TravellingRow]) && LabyrinthDTO->LabyrinthStructure[
		TravellingRow][TravellingColumn + 1] != 1 && LabyrinthDTO->LabyrinthStructure[
		TravellingRow][TravellingColumn + 1] != 3)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			++TravellingColumn;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Up:
		while (TravellingRow - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[TravellingRow - 1][TravellingColumn] != 1 &&
			LabyrinthDTO->LabyrinthStructure[TravellingRow - 1][TravellingColumn] != 3)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			--TravellingRow;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Down:
		while (TravellingRow + 1 < std::size(LabyrinthDTO->LabyrinthStructure) && LabyrinthDTO->LabyrinthStructure[TravellingRow + 1][
		TravellingColumn] != 1 && LabyrinthDTO->LabyrinthStructure[TravellingRow + 1][
		TravellingColumn] != 3)
		{
			if (bIsInRoom && CheckForExit(TravellingRow, TravellingColumn, ChosenDirection))
			{
				UE_LOG(LogTemp, Error, TEXT("Found exit!"));
				FillExitIndexes(TravellingRow, TravellingColumn, ChosenDirection);
				break;
			}
			++TravellingRow;
			if (IsIntersection(TravellingRow, TravellingColumn) && !bIsInRoom) break;
		}
		break;
	case EED_Diagonal:
		FillDiagonalIndexes(TravellingRow, TravellingColumn);
		break;
	default: ;
	}

	const FVector Destination{
		WallSettings::WallOffset * TravellingColumn, WallSettings::WallOffset * TravellingRow, 0.f
	};

	DrawDebugSphere(GetWorld(), Destination, 20.f, 10, FColor::Red, true);
	UE_LOG(LogTemp, Warning, TEXT("Post enemy indexes: %d %d"), TravellingRow, TravellingColumn)
	// Update enemy values by ref
	LastDirection = ChosenDirection;
	Row = TravellingRow;
	Column = TravellingColumn;

	return Destination;
}

void ABaseEnemy::FillFreeDirections(uint8 Row, uint8 Column, TArray<EEnemyDirection>& FreeEnemyDirections)
{
	if (Column - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row][Column - 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row][Column - 1] != 3)
		FreeEnemyDirections.Add(
			EEnemyDirection::EED_Left);
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && LabyrinthDTO->LabyrinthStructure[Row][Column + 1] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row][Column + 1] != 3)
		FreeEnemyDirections.Add(EEnemyDirection::EED_Right);
	if (Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) && LabyrinthDTO->LabyrinthStructure[Row + 1][Column] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row + 1][Column] != 3)
		FreeEnemyDirections.Add(EEnemyDirection::EED_Down);
	if (Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row - 1][Column] != 3) FreeEnemyDirections.Add(EEnemyDirection::EED_Up);


	const bool bDiagonal = IsDiagonal(Row, Column);
	UE_LOG(LogTemp, Warning, TEXT("Diagonal presence: %s"), bDiagonal ? *FString("True") : *FString("False"))

	if (bDiagonal) FreeEnemyDirections.Add(EEnemyDirection::EED_Diagonal);
}

void ABaseEnemy::ChooseNextDirection(TArray<EEnemyDirection>& EnemyDirections, EEnemyDirection& NextDirection,
                                           EEnemyDirection PreviousDirection, uint8 MinIndex, uint8 MaxIndex)
{
	const float SwitchAtIntersecValue = FMath::RandRange(0.f, 1.f);
	const float SwitchToDiagonalValue = FMath::RandRange(0.f, 1.f);

	for (const auto& RandomDirection : EnemyDirections)
		UE_LOG(LogTemp, Warning, TEXT("Free direction: %s"), *UEnum::GetValueAsString(RandomDirection))

	UE_LOG(LogTemp, Warning, TEXT("Previous direction: %s"), *UEnum::GetValueAsString(PreviousDirection))
	// Pick diagonal first
	if (EnemyDirections.Contains(EED_Diagonal) && SwitchToDiagonalValue < EnemySettings::TurnAtDiagonalProbability &&
		PreviousDirection != EED_Diagonal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Switching to diagonal: %f < %f"), SwitchToDiagonalValue,
		       EnemySettings::TurnAtDiagonalProbability)
		NextDirection = EED_Diagonal;
		return;
	}

	if (EnemyDirections.Num() > 1) EnemyDirections.Remove(EED_Diagonal);

	// No free directions, we're stuck 
	if (EnemyDirections.Num() <= 0)
	{
		NextDirection = EED_None;
		return;
	}

	// Blind spot, only way is to go back
	if (EnemyDirections.Num() == 1)
	{
		NextDirection = EnemyDirections[0];
		return;
	}

	// It means that I stopped in a corner
	if (EnemyDirections.Num() >= 2 || SwitchAtIntersecValue < EnemySettings::TurnAtIntersecProbability ||
		PreviousDirection == EED_Diagonal)
	{
		NextDirection = GetIntersecDirection(EnemyDirections, PreviousDirection);
		return;
	}

	NextDirection = PreviousDirection;
}

EEnemyDirection ABaseEnemy::GetOppositeDirection(EEnemyDirection EnemyDirection)
{
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		return EED_Right;
	case EED_Right:
		return EED_Left;
	case EED_Up:
		return EED_Down;
	case EED_Down:
		return EED_Up;
	case EED_Diagonal:
		return EED_Diagonal;
	default: ;
	}

	return EED_None;
}

EEnemyDirection ABaseEnemy::GetIntersecDirection(TArray<EEnemyDirection>& EnemyDirections,
                                                       EEnemyDirection EnemyDirection)
{
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
	case EED_Right:
		if (EnemyDirections.Contains(EED_Up)) return EED_Up;
		if (EnemyDirections.Contains(EED_Down)) return EED_Down;
		break;
	case EED_Up:
	case EED_Down:
		if (EnemyDirections.Contains(EED_Right)) return EED_Right;
		if (EnemyDirections.Contains(EED_Left)) return EED_Left;
		break;
	case EED_Diagonal:
		return EnemyDirections[FMath::RandRange(0, EnemyDirections.Num() - 1)];
	default: ;
	}
	return EED_None;
}

bool ABaseEnemy::IsIntersection(uint8 Row, uint8 Column) const
{
	uint8 Counter{0};
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && LabyrinthDTO->LabyrinthStructure[Row][Column + 1] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row][Column + 1] != 3)
		Counter
			++;
	if (Column - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row][Column - 1] != 1 &&  LabyrinthDTO->LabyrinthStructure[Row][Column - 1] != 3) Counter++;
	if (Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) && LabyrinthDTO->LabyrinthStructure[Row + 1][Column] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row + 1][Column] != 3) Counter++;
	if (Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column] != 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column] != 3) Counter++;

	return Counter > 2;
}

bool ABaseEnemy::IsDiagonal(uint8 Row, uint8 Column) const
{
	if (Column - 1 >= 0 && Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] != 3) return true;
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column
	+ 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column
	+ 1] != 3)
		return true;
	if (Column - 1 >= 0 && Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1]
	!= 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1]
	!= 3)
		return true;
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) &&
		LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] != 3)
		return true;

	return false;
}

bool ABaseEnemy::IsInRoom(uint8 Row, uint8 Column, const TArray<EEnemyDirection>& FreeEnemyDirections)
{
	TArray<EEnemyDiagonalDirection> EnemyDiagonalDirections;
	FillDiagonalMatrix(Row, Column, EnemyDiagonalDirections);

	for (const auto& DiagonalDirection : EnemyDiagonalDirections)
	{
		switch (DiagonalDirection)
		{
		case EEDD_UpperRight:
			if (FreeEnemyDirections.Contains(EED_Up) && FreeEnemyDirections.Contains(EED_Right)) return true;
			break;
		case EEDD_UpperLeft:
			if (FreeEnemyDirections.Contains(EED_Up) && FreeEnemyDirections.Contains(EED_Left)) return true;
			break;
		case EEDD_LowerRight:
			if (FreeEnemyDirections.Contains(EED_Down) && FreeEnemyDirections.Contains(EED_Right)) return true;
			break;
		case EEDD_LowerLeft:
			if (FreeEnemyDirections.Contains(EED_Down) && FreeEnemyDirections.Contains(EED_Left)) return true;
			break;
		default: ;
		}
	}

	return false;
}

bool ABaseEnemy::CheckForExit(uint8 Row, uint8 Column, EEnemyDirection EnemyDirection)
{
	UE_LOG(LogTemp, Warning, TEXT("Checking for exit in: %s Row: %d Column: %d"),
	       *UEnum::GetValueAsString(EnemyDirection), Row, Column);
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		if (LabyrinthDTO->LabyrinthStructure[Row - 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] == 0)
			return
				true;
		if (LabyrinthDTO->LabyrinthStructure[Row + 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1] == 0)
			return
				true;
		break;
	case EED_Right:
		if (LabyrinthDTO->LabyrinthStructure[Row - 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column + 1] == 0)
			return
				true;
		if (LabyrinthDTO->LabyrinthStructure[Row + 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] == 0)
			return
				true;
		break;
	case EED_Up:
		if (LabyrinthDTO->LabyrinthStructure[Row][Column + 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column + 1] == 0)
			return
				true;
		if (LabyrinthDTO->LabyrinthStructure[Row][Column - 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] == 0)
			return
				true;
		break;
	case EED_Down:
		if (LabyrinthDTO->LabyrinthStructure[Row][Column + 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] == 0)
			return
				true;
		if (LabyrinthDTO->LabyrinthStructure[Row][Column - 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1] == 0)
			return
				true;
		break;
	case EED_Diagonal:
		break;
	default: ;
	}

	return false;
}

void ABaseEnemy::FillExitIndexes(uint8& Row, uint8& Column, EEnemyDirection EnemyDirection)
{
	switch (EnemyDirection)
	{
	case EED_None:
		break;
	case EED_Left:
		if (LabyrinthDTO->LabyrinthStructure[Row - 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] == 0)
		{
			Row--;
			Column--;
			break;
		}
		if (LabyrinthDTO->LabyrinthStructure[Row + 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1] == 0)
		{
			Row++;
			Column--;
			break;
		}
		break;
	case EED_Right:
		if (LabyrinthDTO->LabyrinthStructure[Row - 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column + 1] == 0)
		{
			Row--;
			Column++;
			break;
		}
		if (LabyrinthDTO->LabyrinthStructure[Row + 1][Column] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] == 0)
		{
			Row++;
			Column++;
			break;
		}
		break;
	case EED_Up:
		if (LabyrinthDTO->LabyrinthStructure[Row][Column + 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column + 1] == 0)
		{
			Row--;
			Column++;
			break;
		}
		if (LabyrinthDTO->LabyrinthStructure[Row][Column - 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] == 0)
		{
			Row--;
			Column--;
			break;
		}
		break;
	case EED_Down:
		if (LabyrinthDTO->LabyrinthStructure[Row][Column + 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] == 0)
		{
			Row++;
			Column++;
			break;
		}
		if (LabyrinthDTO->LabyrinthStructure[Row][Column - 1] == 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1] == 0)
		{
			Row++;
			Column--;
			break;
		}
		break;
	case EED_Diagonal:
		break;
	default: ;
	}
}

void ABaseEnemy::FillDiagonalIndexes(uint8& Row, uint8& Column)
{
	UE_LOG(LogTemp, Warning, TEXT("Filling diagonal indexes"))

	TArray<EEnemyDiagonalDirection> DiagonalDirections;

	FillDiagonalMatrix(Row, Column, DiagonalDirections);

	switch (DiagonalDirections[FMath::RandRange(0, DiagonalDirections.Num() - 1)])
	{
	case EEDD_UpperRight:
		Row--;
		Column++;
		break;
	case EEDD_UpperLeft:
		Row--;
		Column--;
		break;
	case EEDD_LowerRight:
		Row++;
		Column++;
		break;
	case EEDD_LowerLeft:
		Row++;
		Column--;
		break;
	default: ;
	}
}

void ABaseEnemy::FillDiagonalMatrix(uint8 Row, uint8 Column,
                                          TArray<EEnemyDiagonalDirection>& EnemyDiagonalDirections)
{
	if (Column - 1 >= 0 && Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] != 1 &&
		LabyrinthDTO->LabyrinthStructure[Row - 1][Column - 1] != 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Upper left"))
		EnemyDiagonalDirections.Add(EEDD_UpperLeft);
	}
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column
	+ 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column
	+ 1] != 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Upper right"))
		EnemyDiagonalDirections.Add(EEDD_UpperRight);
	}
	if (Column - 1 >= 0 && Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1]
	!= 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1]
	!= 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom left"))
		EnemyDiagonalDirections.Add(EEDD_LowerLeft);
	}
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) &&
		LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] != 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom right"))
		EnemyDiagonalDirections.Add(EEDD_LowerRight);
	}
}