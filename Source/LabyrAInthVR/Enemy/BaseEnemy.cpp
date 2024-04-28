#include "BaseEnemy.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LabyrAInthVR/Interagibles/StatsChangerComponent.h"
#include "LabyrAInthVR/Network/DTO/LabyrinthDTO.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "LabyrAInthVR/Scene/Config.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Enemy_Log);

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	if (!IsValid(PawnSensingComponent)) return;
	
	PawnSensingComponent->HearingThreshold = 1500.f;
	PawnSensingComponent->SetPeripheralVisionAngle(60.f);
	PawnSensingComponent->SightRadius = 5000.f;
	PawnSensingComponent->bOnlySensePlayers = false;
	
	if (!IsValid(GetCharacterMovement())) return;

	GetCharacterMovement()->MaxAcceleration = 450.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	StatsChangerComponent = CreateDefaultSubobject<UStatsChangerComponent>(TEXT("StatsChangerComponent"));
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultController();

	AIController = Cast<AAIController>(GetController());

	if (!IsValid(AIController)) return;

	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ThisClass::OnMoveFinished);

	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);

	if (!IsValid(PawnSensingComponent)) return;

	PawnSensingComponent->Activate();
	PawnSensingComponent->SetSensingUpdatesEnabled(true);
	
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ThisClass::OnSeePawn);
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &ThisClass::OnHearNoise);

	NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(this);

	if (!IsValid(NavigationSystemV1)) return;

	NavigationSystemV1->OnNavigationGenerationFinishedDelegate.AddDynamic(
		this, &ThisClass::NavMeshFinishedBuilding);
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

void ABaseEnemy::Freeze(uint8 Time)
{
	LastKnownEnemyState = EnemyState;
	EnemyState = EES_Frozen;
	GetWorldTimerManager().SetTimer(FreezingTimerHandle, this, &ThisClass::FreezeTimerFinished, Time, false);
}

void ABaseEnemy::NavMeshFinishedBuilding(ANavigationData* NavigationData)
{
	if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s: Enemy set to EES_Idle, ready for patrolling"), *GetName());
	EnemyState = EES_Idle;
}

void ABaseEnemy::PatrollingTimerFinished()
{
	if (EnemyState == EES_Attacking || !IsValid(AIController)) return;

	if (EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s: Patrolling timer finished"), *GetName());

	EnemyState = EES_Patrolling;

	End = GetNextDestination(MatrixRow, MatrixColumn, LastKnownDirection);
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(End);
	MoveRequest.SetAcceptanceRadius(0.f);
	AIController->MoveTo(MoveRequest);
}

void ABaseEnemy::OnSeePawn(APawn* Pawn)
{
	if (EnemyState > EES_Patrolling) return;

	if (EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> seen pawn: %s"), *GetName(),  *Pawn->GetName());
	
	SeenCharacter = SeenCharacter == nullptr ? Cast<AMainCharacter>(Pawn) : SeenCharacter;
	
	if (!IsValid(SeenCharacter) || !IsCharacterOnNavMesh() || !SeenCharacter->IsAlive()) return;

	if (EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Initiating chase action to: %s from ABaseEnemy::OnSeePawn"), *GetName(),  *SeenCharacter->GetName());
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

	UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Heard sound, going to the sound location"), *GetName())
}

void ABaseEnemy::FreezeTimerFinished()
{
	EnemyState = LastKnownEnemyState;
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
	if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Move finished"), *GetName());
}

// Whenever we chased we have to update the matrix position and we do it based on the X Y coordinates
void ABaseEnemy::UpdateMatrixPosition()
{
	LastKnownDirection = EED_None;
	//UE_LOG(LogTemp, Warning, TEXT("Previous position: %d %d"), MatrixRow, MatrixColumn);
	float XPos = GetActorLocation().X;
	float YPos = GetActorLocation().Y;

	MatrixColumn = XPos / WallSettings::WallOffset;
	MatrixRow = YPos / WallSettings::WallOffset;

	/*UE_LOG(LogTemp, Warning, TEXT("X Position: %f - Modulus: %f - Index: %d"), XPos,
	       FMath::Fmod(XPos, WallSettings::WallOffset), MatrixColumn);
	UE_LOG(LogTemp, Warning, TEXT("Y Position: %f - Modulus: %f - Index: %d"), YPos,
	       FMath::Fmod(YPos, WallSettings::WallOffset), MatrixRow);*/

	if (FMath::Fmod(XPos, WallSettings::WallOffset) > WallSettings::WallOffset / 2)
	{
		MatrixColumn++;
	}

	if (FMath::Fmod(YPos, WallSettings::WallOffset) > WallSettings::WallOffset / 2)
	{
		MatrixRow++;
	}

	//UE_LOG(LogTemp, Warning, TEXT("New position: %d %d"), MatrixRow, MatrixColumn);

	FVector NewPos{MatrixColumn * WallSettings::WallOffset, MatrixRow * WallSettings::WallOffset, 0.f};
	DrawDebugSphere(GetWorld(), NewPos, 20.f, 15, FColor::Green, true);
}

void ABaseEnemy::StartPatrolling()
{
	if (EnemyState != EES_Idle || GetWorldTimerManager().TimerExists(PatrollingTimerHandle) || AIController == nullptr)
		return;

	const float RandomWaitTime = FMath::FRandRange(MinPatrolTimer, MaxPatrolTimer);
	if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Random wait time: %f"), *GetName(), RandomWaitTime);
	GetWorldTimerManager().SetTimer(PatrollingTimerHandle, this, &ThisClass::PatrollingTimerFinished, RandomWaitTime,
	                                false);
}

void ABaseEnemy::Chase()
{
	if (!IsValid(AIController)) return;
	
	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
	EnemyState = EES_Chasing;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(SeenCharacter);
	MoveRequest.SetAcceptanceRadius(0.f);
	AIController->MoveTo(MoveRequest);
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

bool ABaseEnemy::IsFacing()
{
	if (!IsValid(SeenCharacter)) return false;
	
	return UKismetMathLibrary::DegAcos(
			FVector::DotProduct(SeenCharacter->GetActorForwardVector(), GetActorForwardVector())) >
		Misc::FacingEnemyDegrees;
}

bool ABaseEnemy::IsCharacterOnNavMesh()
{
	if (!IsValid(NavigationSystemV1)) return false;
	FNavLocation NavLocation;
	return NavigationSystemV1->ProjectPointToNavigation(SeenCharacter->GetActorLocation(), NavLocation);
}

void ABaseEnemy::PlayMontage(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance) || !IsValid(MontageToPlay)) return;
	
	
	FName SectionName{*FString::Printf(TEXT("%d"), FMath::RandRange(1, MontageToPlay->GetNumSections()))};
	if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Playing montage with section: %s %s"), *GetName(), *MontageToPlay->GetName(), *SectionName.ToString());
	AnimInstance->Montage_Play(MontageToPlay);
	AnimInstance->Montage_JumpToSection(SectionName);
}

void ABaseEnemy::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                               AController* InstigatedBy, AActor* DamageCauser)
{
	if (EnemyState == EES_Dead || !IsValid(DamageCauser)) return;

	if (EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Received %f damage by: %s"), *GetName(), Damage, *DamageCauser->GetName());
	
	if (bHasShield)
	{
		DectivateShield();
		if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Received damage but had shield, shield is destroyed"), *GetName());
		return;
	}
	
	Health -= Damage;

	if (BloodEffect == nullptr) return;
	
	FVector EffectSpawn{
		GetActorLocation() + FVector{0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.75, 0.f}
	};
	
	UGameplayStatics::SpawnEmitterAtLocation(this, BloodEffect, EffectSpawn);

	if (Health > 0) return;
	
	GetWorldTimerManager().ClearTimer(PatrollingTimerHandle);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AIController->StopMovement();
	EnemyState = EES_Dead;
	PlayMontage(DeathMontage);
}

FVector ABaseEnemy::GetNextDestination(uint8& Row, uint8& Column, EEnemyDirection& LastDirection)
{
	if (!IsValid(LabyrinthDTO))
	{
		return FVector{};
	}
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
		if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Standing in a room"), *GetName());
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

	if (EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Chosen direction: %s"), *GetName(), *UEnum::GetValueAsString(ChosenDirection));

	// Find the (X,Y) for the chosen direction, stop at intersections
	uint8 TravellingRow{Row};
	uint8 TravellingColumn{Column};

	//UE_LOG(LogTemp, Warning, TEXT("Current enemy indexes: %d %d"), TravellingRow, TravellingColumn)

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
				if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Found exit from room"), *GetName());
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
				if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Found exit from room"), *GetName());
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
				if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Found exit from room"), *GetName());
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
				if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Found exit from room"), *GetName());
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
	if (EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Diagonal presence: %s"), *GetName(), bDiagonal ? *FString("True") : *FString("False"));

	if (bDiagonal) FreeEnemyDirections.Add(EEnemyDirection::EED_Diagonal);
}

void ABaseEnemy::ChooseNextDirection(TArray<EEnemyDirection>& EnemyDirections, EEnemyDirection& NextDirection,
                                           EEnemyDirection PreviousDirection, uint8 MinIndex, uint8 MaxIndex)
{
	const float SwitchAtIntersecValue = FMath::RandRange(0.f, 1.f);
	const float SwitchToDiagonalValue = FMath::RandRange(0.f, 1.f);

	for (const auto& RandomDirection : EnemyDirections)
		if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Free direction: %s"), *GetName(), *UEnum::GetValueAsString(RandomDirection));

	if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Display, TEXT("%s -> Previous direction: %s"), *GetName(), *UEnum::GetValueAsString(PreviousDirection));
	// Pick diagonal first
	if (EnemyDirections.Contains(EED_Diagonal) && SwitchToDiagonalValue < EnemySettings::TurnAtDiagonalProbability &&
		PreviousDirection != EED_Diagonal)
	{
		NextDirection = EED_Diagonal;
		return;
	}

	if (EnemyDirections.Num() > 1) EnemyDirections.Remove(EED_Diagonal);

	// No free directions, we're stuck 
	if (EnemyDirections.Num() <= 0)
	{
		NextDirection = EED_None;
		if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Warning, TEXT("%s -> No free direction"), *GetName());
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

void ABaseEnemy::SetLabyrinthMatrix(ULabyrinthDTO* LabyrinthDTOReference)
{
	if (!IsValid(LabyrinthDTOReference))
	{
		if(EnemySettings::bEnableLog) UE_LOG(LabyrAInthVR_Enemy_Log, Error, TEXT("%s -> LabyrinthDTO in ABaseEnemy::SetLabyrinthMatrix is NULL"), *GetName());
		return;
	}


	LabyrinthDTO = LabyrinthDTOReference;
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
		EnemyDiagonalDirections.Add(EEDD_UpperLeft);
	}
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && Row - 1 >= 0 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column
	+ 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row - 1][Column
	+ 1] != 3)
	{
		EnemyDiagonalDirections.Add(EEDD_UpperRight);
	}
	if (Column - 1 >= 0 && Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1]
	!= 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column - 1]
	!= 3)
	{
		EnemyDiagonalDirections.Add(EEDD_LowerLeft);
	}
	if (Column + 1 < std::size(LabyrinthDTO->LabyrinthStructure[Row]) && Row + 1 < std::size(LabyrinthDTO->LabyrinthStructure) &&
		LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] != 1 && LabyrinthDTO->LabyrinthStructure[Row + 1][Column + 1] != 3)
	{
		EnemyDiagonalDirections.Add(EEDD_LowerRight);
	}
}