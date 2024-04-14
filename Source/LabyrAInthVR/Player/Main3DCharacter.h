#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "Main3DCharacter.generated.h"

class USpotLightComponent;
class UCameraComponent;

UCLASS()
class LABYRAINTHVR_API AMain3DCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMain3DCharacter();

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

	UPROPERTY(EditAnywhere, Category=Gameplay)
	USpotLightComponent* Flashlight;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* MoveInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* LookInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* JumpInputAction;

	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* FlashlightInputAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ToggleFlashlight(const FInputActionValue& Value);
};
