#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LabyrAInthVR/Interagibles/StatsChangerComponent.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseEnemy.generated.h"

class UStatsChangerComponent;
class AVRMainCharacter;
class AMainCharacter;
class AMockedCharacter;
class UNavigationSystemV1;
class ULabyrinthDTO;
class UBoxComponent;
class ALabyrinthParser;
class UPawnSensingComponent;
class AAIController;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Enemy_Log, Display, All);

UENUM()
enum EEnemyState : uint8
{
	EES_WaitingForNav UMETA(DisplayName = "Waiting for Nav Mesh"),
	EES_Idle UMETA(DisplayName = "Idle"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Hold UMETA(DisplayName = "Hold"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Dead UMETA(DisplayName = "Dead")
};

UENUM()
enum EEnemyDirection
{
	EED_None UMETA(DisplayName = "None"),
	EED_Left UMETA(DisplayName = "Left"),
	EED_Right UMETA(DisplayName = "Right"),
	EED_Up UMETA(DisplayName = "Up"),
	EED_Down UMETA(DisplayName = "Down"),
	EED_Diagonal UMETA(DisplayName = "Diagonal")
};

UENUM()
enum EEnemyDiagonalDirection
{
	EEDD_UpperRight UMETA(DisplayName = "Upper right"),
	EEDD_UpperLeft UMETA(DisplayName = "Upper left"),
	EEDD_LowerRight UMETA(DisplayName = "Lower right"),
	EEDD_LowerLeft UMETA(DisplayName = "Lower left"),
};

USTRUCT()
struct FEnemyDirection
{
	GENERATED_BODY()
	EEnemyDirection EnemyDirection;
	FVector Goal;
};

UCLASS()
class LABYRAINTHVR_API ABaseEnemy : public ACharacter, public IDamageableActor
{
	GENERATED_BODY()

public:
	ABaseEnemy();
	void SetMatrixPosition(uint8 Row, uint8 Column);
	float GetSpeed();
protected:
	virtual void BeginPlay() override;
	void StartPatrolling();
	void RotateToCharacter();
	virtual void PlayMontage(UAnimMontage* MontageToPlay);
	void Chase();
	void UpdateMatrixPosition();
	bool IsFacing();
	bool IsCharacterOnNavMesh();
	
	EEnemyState EnemyState{EEnemyState::EES_WaitingForNav};

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                           AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	UNavigationSystemV1* NavigationSystemV1;

	UPROPERTY()
	AMainCharacter* SeenCharacter;
	
	UPROPERTY(EditAnywhere, Category="Settings|Health")
	float Health{100.f};

	UPROPERTY(EditAnywhere, Category="Stats")
	UStatsChangerComponent* StatsChangerComponent;

	UPROPERTY(EditAnywhere, Category="Stats")
	TArray<FStatsType> Stats;
private:
	UPROPERTY()
	ULabyrinthDTO* LabyrinthDTO;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category=PawnSensing)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, Category=Settings)
	float MinPatrolTimer{1.f};

	UPROPERTY(EditAnywhere, Category=Settings)
	float MaxPatrolTimer{3.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	float MaxHealth{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Shield")
	float Shield{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Shield")
	float MaxShield{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	UParticleSystem* BloodEffect;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* DeathMontage;

	bool bRotated{false};
	bool bHasShield{true};
	FVector End{};
	FTimerHandle PatrollingTimerHandle;

	uint8 MatrixRow{0};
	uint8 MatrixColumn{0};

private:
	UFUNCTION()
	void NavMeshFinishedBuilding(ANavigationData* NavigationData);

	UFUNCTION()
	void PatrollingTimerFinished();

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

	void OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult);

	EEnemyDirection LastKnownDirection{EED_None};

	EEnemyDirection GetIntersecDirection(TArray<EEnemyDirection>& EnemyDirections,
	                                     EEnemyDirection EnemyDirection);
	EEnemyDirection GetOppositeDirection(EEnemyDirection EnemyDirection);
	void ChooseNextDirection(TArray<EEnemyDirection>& EnemyDirections, EEnemyDirection& NextDirection,
	                         EEnemyDirection PreviousDirection, uint8 MinIndex, uint8 MaxIndex);
	void FillFreeDirections(uint8 Row, uint8 Column, TArray<EEnemyDirection>& FreeEnemyDirections);
	FVector GetNextDestination(uint8& Row, uint8& Column, EEnemyDirection& LastDirection);
	void FillDiagonalMatrix(uint8 Row, uint8 Column,
	                        TArray<EEnemyDiagonalDirection>& EnemyDiagonalDirections);
	void FillDiagonalIndexes(uint8& Row, uint8& Column);
	void FillExitIndexes(uint8& Row, uint8& Column, EEnemyDirection EnemyDirection);
	bool CheckForExit(uint8 Row, uint8 Column, EEnemyDirection EnemyDirection);
	bool IsInRoom(uint8 Row, uint8 Column, const TArray<EEnemyDirection>& FreeEnemyDirections);
	bool IsDiagonal(uint8 Row, uint8 Column) const;
	bool IsIntersection(uint8 Row, uint8 Column) const;

public:
	void SetLabyrinthMatrix(ULabyrinthDTO* LabyrinthDTOReference);
	FORCEINLINE void ActivateShield() { bHasShield = true; }
	FORCEINLINE void DectivateShield() { bHasShield = false; }
};
