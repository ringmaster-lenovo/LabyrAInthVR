#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MockedCharacter.generated.h"

UCLASS()
class LABYRAINTHVR_API AMockedCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMockedCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsAlive{true};

public:
	FORCEINLINE bool IsAlive() {return bIsAlive; }
};
