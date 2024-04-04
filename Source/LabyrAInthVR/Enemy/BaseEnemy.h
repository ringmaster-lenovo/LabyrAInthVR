#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseEnemy.generated.h"

class AMockedCharacter;
class UNavigationSystemV1;
class ULabyrinthDTO;
class UBoxComponent;
class ALabyrinthParser;
class UPawnSensingComponent;
class AAIController;

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
class LABYRAINTHVR_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							 AController* InstigatedBy, AActor* DamageCauser);

public:
	virtual void Tick(float DeltaTime) override;
	void SetMatrixPosition(uint8 Row, uint8 Column);
	float GetSpeed();

private:
	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	ULabyrinthDTO* LabyrinthDTO;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category=PawnSensing)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, Category=Weapon)
	FName WeaponSocketName{};

	UPROPERTY(EditAnywhere, Category=Settings)
	float MinPatrolTimer{1.f};

	UPROPERTY(EditAnywhere, Category=Settings)
	float MaxPatrolTimer{3.f};

	UPROPERTY(EditAnywhere, Category=Settings)
	float ChaseDistance{500.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float AttackTimer{2.f};

	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float AttackDistance{500.f};
	
	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float AttackSpeed{1.f};
	
	UPROPERTY(EditAnywhere, Category="Settings|Attack")
	float AttackDamage{50.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	float Health{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	float MaxHealth{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Shield")
	float Shield{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Shield")
	float MaxShield{100.f};
	
	UPROPERTY(EditAnywhere, Category="Settings|Health")
	UParticleSystem* BloodEffect;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* DeathMontage;

	UPROPERTY()
	AMockedCharacter* SeenCharacter;

	UPROPERTY()
	UNavigationSystemV1* NavigationSystemV1;

	bool bRotated{false};
	bool bCanAttack{true};
	bool bHasShield{true};
	FVector End{};
	EEnemyState EnemyState{EEnemyState::EES_WaitingForNav};
	FTimerHandle PatrollingTimerHandle;
	FTimerHandle AttackingTimerHandle;

	uint8 MatrixRow{0};
	uint8 MatrixColumn{0};

private:
	UFUNCTION()
	void NavMeshFinishedBuilding(ANavigationData* NavigationData);

	UFUNCTION()
	void PatrollingTimerFinished();

	UFUNCTION()
	void AttackingTimerFinished();

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

	void OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult);

	void UpdateMatrixPosition();

	void StartPatrolling();

	void Chase();

	void Attack();

	void HoldPosition();

	void CheckAttack();

	void RotateToCharacter();

	bool CanExecuteAction();

	float GetDistanceToCharacter();

	bool IsAttacking();

	bool IsFacing();

	bool IsCharacterOnNavMesh();

	void PlayMontage(UAnimMontage* MontageToPlay);
	
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
	FORCEINLINE void SetLabyrinthMatrix(ULabyrinthDTO* LabyrinthDTOReference) {LabyrinthDTO = LabyrinthDTOReference; }
	FORCEINLINE void ActivateShield() { bHasShield = true; }
	FORCEINLINE void DectivateShield() { bHasShield = false; }
};
