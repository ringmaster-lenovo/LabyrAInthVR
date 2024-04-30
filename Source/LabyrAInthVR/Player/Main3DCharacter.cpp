#include "Main3DCharacter.h"

#include "BasePlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Pickups/BasePickup.h"
#include "LabyrAInthVR/Projectiles/Projectile.h"
#include "Particles/ParticleSystem.h"

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
	
	Flashlight->SetupAttachment(FirstPersonCamera);
}

void AMain3DCharacter::ResetWeapon()
{
	Super::ResetWeapon();
	bHasWeapon = false;
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

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		//EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(PickupInputAction, ETriggerEvent::Triggered, this, &ThisClass::PickupObject);
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

void AMain3DCharacter::PickupObject(const FInputActionValue& Value)
{
	if (!IsValid(OverlappingPickup) || !IsValid(FirstPersonMesh) || !IsValid(OverlappingPickup->GetWeapon())) return;
	
	const USkeletalMeshSocket* Socket = FirstPersonMesh->GetSocketByName(FName("GripPoint"));

	ABasePickup* AuxPickup {nullptr};
	FVector PickupLocation{OverlappingPickup->GetActorLocation()};
	
	// I already have an equipped weapon
	if (IsValid(EquippedWeapon))
	{
		AuxPickup = EquippedWeapon->GetPickup();
		EquippedWeapon->Destroy();
	}

	UE_LOG(LabyrAInthVR_Player_Log, Warning, TEXT("Current pickup: %s"), *OverlappingPickup->GetName())
	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(OverlappingPickup->GetWeapon());
	
	if (!IsValid(EquippedWeapon)) return;

	Socket->AttachActor(EquippedWeapon, FirstPersonMesh);
	bHasWeapon = true;
	EquippedWeapon->SetPickup(OverlappingPickup);
	OverlappingPickup->SetHasBeenFound();
	OverlappingPickup->SetActorHiddenInGame(true);
	OverlappingPickup->SetActorEnableCollision(false);  // BEWARE: after this line OverlappingPickup is not valid anymore

	if (!IsValid(AuxPickup)) return;
	
	AuxPickup->SetActorHiddenInGame(false);
	AuxPickup->SetActorEnableCollision(true);
	AuxPickup->SetActorLocation(PickupLocation);
}

/*void AMain3DCharacter::PickupWeapon()
{
	Super::PickupWeapon();
}*/

