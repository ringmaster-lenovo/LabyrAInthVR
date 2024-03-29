#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseEnemy.generated.h"

class UBoxComponent;
class ALabyrinthParser;
class UPawnSensingComponent;
class AAIController;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Enemy_Log, Display, All);

UENUM()
enum EEnemyState : uint8
{
	Ees_WaitingForNav UMETA(DisplayName = "Waiting for Nav Mesh"),
	Ees_Idle UMETA(DisplayName = "Idle"),
	Ees_Patrolling UMETA(DisplayName = "Patrolling"),
	Ees_Chasing UMETA(DisplayName = "Chasing"),
	Ees_Hold UMETA(DisplayName = "Hold"),
	Ees_Attacking UMETA(DisplayName = "Attacking"),
	Ees_Dead UMETA(DisplayName = "Dead")
};

UENUM()
enum EEnemyDirection
{
	Eed_None UMETA(DisplayName = "None"),
	Eed_Left UMETA(DisplayName = "Left"),
	Eed_Right UMETA(DisplayName = "Right"),
	Eed_Up UMETA(DisplayName = "Up"),
	Eed_Down UMETA(DisplayName = "Down"),
	Eed_Diagonal UMETA(DisplayName = "Diagonal")
};

UENUM()
enum EEnemyDiagonalDirection
{
	Eedd_UpperRight UMETA(DisplayName = "Upper right"),
	Eedd_UpperLeft UMETA(DisplayName = "Upper left"),
	Eedd_LowerRight UMETA(DisplayName = "Lower right"),
	Eedd_LowerLeft UMETA(DisplayName = "Lower left"),
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

public:
	virtual void Tick(float DeltaTime) override;
	void SetMatrixPosition(uint8 Row, uint8 Column);
	float GetSpeed() const;

private:
	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	ALabyrinthParser* LabyrinthParser;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category=PawnSensing)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, Category=Weapon)
	UBoxComponent* WeaponBoxComponent;

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
	float AttackDamage{50.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	float Health{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	float MaxHealth{100.f};

	UPROPERTY(EditAnywhere, Category="Settings|Health")
	UParticleSystem* BloodEffect;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category=Animation)
	UAnimMontage* DeathMontage;

	UPROPERTY()
	ACharacter* SeenCharacter;

	bool bRotated{false};
	bool bCanAttack{true};
	FVector End{};
	EEnemyState EnemyState{EEnemyState::Ees_WaitingForNav};
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
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(bool bEnabled);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							 AController* InstigatedBy, AActor* DamageCauser);

	void OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& PathFollowingResult);

	void UpdateMatrixPosition();

	void StartPatrolling();

	void Chase();

	void Attack();

	void HoldPosition();

	void CheckAttack();

	void RotateToCharacter();

	float GetDistanceToCharacter();

	bool IsAttacking();

	bool IsFacing();

	void PlayMontage(UAnimMontage* MontageToPlay);
	
	EEnemyDirection LastKnownDirection{Eed_None};
};
