#include "Main3DCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"

AMain3DCharacter::AMain3DCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetRelativeLocation(FVector{-10.f, 0.f, 60.f});
	FirstPersonCamera->SetProjectionMode(ECameraProjectionMode::Perspective);
	FirstPersonCamera->SetFieldOfView(90.f);
	FirstPersonCamera->bUsePawnControlRotation = 1;
	FirstPersonCamera->SetupAttachment(GetRootComponent());
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);

	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetAttenuationRadius(500000.f);
	Flashlight->SetOuterConeAngle(25.f);
	Flashlight->SetupAttachment(FirstPersonCamera);
}

void AMain3DCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMain3DCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMain3DCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(FlashlightInputAction, ETriggerEvent::Triggered, this, &ThisClass::ToggleFlashlight);
	}
}

void AMain3DCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Vector = Value.Get<FVector2D>();
	AddMovementInput(GetActorRightVector(), Vector.X);
	AddMovementInput(GetActorForwardVector(), Vector.Y);
}

void AMain3DCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Vector = Value.Get<FVector2D>();
	AddControllerYawInput(Vector.X);
	AddControllerPitchInput(Vector.Y);
}

void AMain3DCharacter::ToggleFlashlight(const FInputActionValue& Value)
{
	if(!IsValid(Flashlight)) return;

	Flashlight->SetVisibility(!Flashlight->GetVisibleFlag());
}

