#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "LabyrAInthVR/Interfaces/FreezableActor.h"
#include "LabyrAInthVR/Interfaces/MovableActor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "Trap.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Trap_Log, Display, All);
UCLASS(Blueprintable, BlueprintType)
class LABYRAINTHVR_API ATrap : public AActor, public IDamageableActor, public ISpawnableActor, public IMovableActor, public IFreezableActor
{
	GENERATED_BODY()

public:
	ATrap();
	
	/** Sound to play on collect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USoundBase* CollectSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USoundBase* AlertSound;

	UFUNCTION(BlueprintCallable)
	virtual void Freeze(int32 Time) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnFreeze"))
	void ReceiveFreeze();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnFreezeResume"))
	void ReceiveFreezeResume();

	UFUNCTION(BlueprintCallable)
	void OnEnterSensingArea(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnLeaveSensingArea(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnEnterDamageArea(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnLeaveDamageArea(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	bool IsTrapWorking();
	
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* Actor, float Damage);

	UFUNCTION(BlueprintCallable)
	void ApplyDamageToAll(float Damage);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bCharacterInSensingArea{false};
	bool bIsFrozen{false};
	TArray<AActor*> ActorInRange;

	FTimerHandle FreezingTimerHandle;

	void DamagePlayer(AActor* Actor, float Damage);
	void DamageEnemy(AActor* Actor, float Damage);

	UFUNCTION()
	void FreezeTimerFinished();
	
};
