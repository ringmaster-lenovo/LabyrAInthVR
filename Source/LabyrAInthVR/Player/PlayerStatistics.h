#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatistics.generated.h"

class AMainCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_PlayerStatistics_Log, Display, All);

UENUM()
enum EStatModifier : uint8
{
	Esm_Health UMETA(DisplayName = "Health"),
	Esm_Speed UMETA(DisplayName = "Speed"),
	Esm_Armor UMETA(DisplayName = "Armor")
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
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float DefaultSpeed{650.f};
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Speed{650.f};
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bHasShield{false};
	
	FTimerHandle TimerHandle;
	FTimerHandle DefaultValueTimerHandle;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float LevelTime {0.f};

	UFUNCTION()
	void UpdateTimer();

	UFUNCTION()
	void ResetToDefaultValue(EStatModifier Stat);

	void UpdateSpeed(float NewSpeed);
public:
	UFUNCTION(BlueprintCallable)
	void ChangeStatFloat(EStatModifier Stat, float Amount);

	UFUNCTION(BlueprintCallable)
	void ChangeStatBool(EStatModifier Stat, bool bEnable);

	UFUNCTION(BlueprintCallable)
	void ChangeTimedStat(EStatModifier Stat, float Amount, float Time);

	template<typename T>
	T GetStat(EStatModifier Stat);
	
	void StartLevelTimer();
	void StopLevelTimer();
	float GetLevelTime();
	float GetDefaultHealth();
	float GetCurrentWeaponDamage();
	FPlayerTime GetPlayerTime();
	void ResetStats();
	
	FORCEINLINE bool IsAlive() { return Health > 0; }
	FORCEINLINE float GetPlayerRawTime() {return LevelTime; }
	FORCEINLINE bool HasShield() { return bHasShield; }
	FORCEINLINE void ActivateShield() {bHasShield = true;}
	FORCEINLINE void DeactivateShield() {bHasShield = false; }
};

template <typename T>
T UPlayerStatistics::GetStat(EStatModifier Stat)
{
	switch(Stat) {
	case Esm_Health:
		return Health;
	case Esm_Speed:
		return Speed;
	case Esm_Armor:
		return bHasShield;
	}

	return false;
}
