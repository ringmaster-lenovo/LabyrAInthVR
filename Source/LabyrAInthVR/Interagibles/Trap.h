#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LabyrAInthVR/Player/PlayerStatistics.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "LabyrAInthVR/Interfaces/FreezableActor.h"
#include "LabyrAInthVR/Interfaces/MovableActor.h"
#include "LabyrAInthVR/Interfaces/SpawnableActor.h"
#include "Trap.generated.h"

UCLASS()
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
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};
