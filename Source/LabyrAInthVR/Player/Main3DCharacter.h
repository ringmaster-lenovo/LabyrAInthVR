#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "MainCharacter.h"
#include "Main3DCharacter.generated.h"

class AProjectile;
class USpotLightComponent;
class UCameraComponent;

UCLASS()
class LABYRAINTHVR_API AMain3DCharacter : public AMainCharacter
{
	GENERATED_BODY()

public:
	AMain3DCharacter();
	virtual void ResetWeapon() override;
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, Category=Mesh)
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(EditAnywhere, Category=Gameplay)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* MoveInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* LookInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* JumpInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* PickupInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* ReleasePickupInputAction;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PickupObject(const FInputActionValue& Value);

	//virtual void PickupWeapon(const FInputActionValue& Value) override;
public:
	FORCEINLINE bool GetHasWeapon() {return bHasWeapon; }
};
