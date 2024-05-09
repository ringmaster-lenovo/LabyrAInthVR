#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatistics.generated.h"

class AMainCharacter;
class AMusicController;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_PlayerStatistics_Log, Display, All);

UENUM()
enum EStatModifier : uint8
{
	Esm_Health UMETA(DisplayName = "Health"),
	Esm_Speed UMETA(DisplayName = "Speed"),
	Esm_Shield UMETA(DisplayName = "Shield")
};

USTRUCT(Blueprintable)
struct FPlayerTime
{
	GENERATED_BODY()
	
	int CurrentHours = 0;
	int CurrentMinutes = 0;
	int CurrentSeconds = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LABYRAINTHVR_API UPlayerStatistics : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerStatistics();

	DECLARE_MULTICAST_DELEGATE(FPlayerDiedEvent);
	FPlayerDiedEvent OnPlayerDied;
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	friend class AMainCharacter;

	UPROPERTY()
	AMainCharacter* MainCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float DefaultHealth{100.f};

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Health{100.f};
	
	float CurrentSpeed;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bHasShield{false};
	
	FTimerHandle TimerHandle;
	FTimerHandle DefaultValueTimerHandle;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float LevelTime {0.f};

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float LevelTimer {0.f};

	UFUNCTION()
	void UpdateTimer();

	UFUNCTION()
	void ResetThisPowerUpSpeedModifier(EStatModifier Stat, float Amount);

	void UpdateSpeed();

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundCue* FootstepsWalk;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundCue* FootstepsRun;

	UPROPERTY(EditAnywhere, Category="Sounds")
	float WalkSoundInterval{0.5f};
	
	UPROPERTY(EditAnywhere, Category="Sounds")
	float RunSoundInterval{0.3f};

	AMusicController* MusicController;
	
	UFUNCTION()
	void PlayFootstepSound();

	FTimerHandle FootstepsSoundWalkTimerHandle;
	FTimerHandle FootstepsSoundRunTimerHandle;

	UPROPERTY(EditAnywhere, Category="Speed")
	float WalkSpeed = 400.f;

	float BaseRunSpeedModifier = 200.f;

	float InternalRunModifier;
	
	bool bIsRunning {false};

	float RunSpeedModifier {0.f};

	float SpeedTrapModifier {0.f};

	float SpeedPowerupModifier {0.f};
	
public:
	UFUNCTION(BlueprintCallable)
	void ChangeStatFloat(EStatModifier Stat, float Amount);

	UFUNCTION(BlueprintCallable)
	void ChangeStatBool(EStatModifier Stat, bool bEnable);

	UFUNCTION(BlueprintCallable)
	void ChangeTimedStat(EStatModifier Stat, float Amount, float Time);

	template<typename T>
	T GetStat(EStatModifier Stat);

	float GetDefaultHealth() const;

	void SetLevelTimer(const float Time) { LevelTimer = Time; }
	
	void StartLevelTimer();
	void StopLevelTimer();
	float GetLevelTime();
	float GetLevelTimer();
	float GetCurrentWeaponDamage();
	FPlayerTime GetPlayerTime();
	void ResetSpeed();
	void ResetStats();
	
	FORCEINLINE bool IsAlive() { return Health > 0; }
	FORCEINLINE float GetPlayerRawTime() {return LevelTime; }
	FORCEINLINE bool HasShield() { return bHasShield; }
	FORCEINLINE void ActivateShield() {bHasShield = true;}
	FORCEINLINE void DeactivateShield() {bHasShield = false; }

	UFUNCTION(BlueprintCallable)
	void SetSpeedModifier(float NewSpeedModifier);
	void Sprint(bool bSprint);
};

template <typename T>
T UPlayerStatistics::GetStat(EStatModifier Stat)
{
	switch(Stat) {
	case Esm_Health:
		return Health;
	case Esm_Speed:
		return CurrentSpeed;
	case Esm_Shield:
		return bHasShield;
	}

	return false;
}
