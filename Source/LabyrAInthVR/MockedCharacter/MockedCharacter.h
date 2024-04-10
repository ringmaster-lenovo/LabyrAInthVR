#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LabyrAInthVR/Interfaces/DamageableActor.h"
#include "MockedCharacter.generated.h"

UCLASS()
class LABYRAINTHVR_API AMockedCharacter : public ACharacter, public IDamageableActor
{
	GENERATED_BODY()

public:
	AMockedCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							   AController* InstigatedBy, AActor* DamageCauser);
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsAlive{true};

public:
	FORCEINLINE bool IsAlive() {return bIsAlive; }
};
