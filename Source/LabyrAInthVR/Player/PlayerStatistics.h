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
	float MaxHealth{100.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Health{100.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Speed{650.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bHasShield{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FPlayerTime PlayerTime;
	
	FTimerHandle TimerHandle;
	FTimerHandle DefaultValueTimerHandle;
	float RawTime {0.f};
	
	float DefaultSpeed {Speed};

	UFUNCTION()
	void UpdateTimer();

	UFUNCTION()
	void ResetToDefaultValue(EStatModifier Stat);
public:
	void ChangeStat(EStatModifier Stat, float Amount);
	void ChangeStat(EStatModifier Stat, float Amount, float Time);
	float GetStat(EStatModifier Stat);
	void StartRawTimer();
	FPlayerTime GetPlayerTime();
	
	FORCEINLINE bool IsAlive() { return Health > 0; }
	FORCEINLINE void SetPlayerName(FString Name) { PlayerName = Name; }
	FORCEINLINE FString GetPlayerName() {return PlayerName; }
	FORCEINLINE float GetPlayerRawTime() {return RawTime; }
	FORCEINLINE bool HasShield() { return bHasShield; }
	FORCEINLINE void ActivateShield() {bHasShield = true;}
	FORCEINLINE void DeactivateShield() {bHasShield = false; }
};
