#include "Main3DCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LabyrAInthVR/Projectiles/Projectile.h"

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
		EnhancedInputComponent->BindAction(PickupInputAction, ETriggerEvent::Triggered, this, &ThisClass::PickupObject);
		EnhancedInputComponent->BindAction(ShootInputAction, ETriggerEvent::Triggered, this, &ThisClass::Shoot);

		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &ThisClass::Sprint, true);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Canceled, this, &ThisClass::Sprint, false);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &ThisClass::Sprint, false);
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

void AMain3DCharacter::PickupObject(const FInputActionValue& Value)
{
	if(!IsValid(OverlappedPickup) || !IsValid(FirstPersonMesh) || Weapon == nullptr) return;

	//UE_LOG(LogTemp, Warning, TEXT("Pickup is available, picking it."))
	OverlappedPickup->Destroy();
	bHasWeapon = true;

	const USkeletalMeshSocket* Socket = FirstPersonMesh->GetSocketByName(FName("GripPoint"));
	SpawnedWeapon = Cast<AActor>(GetWorld()->SpawnActor(Weapon));

	if(!IsValid(SpawnedWeapon)) return;
	
	Socket->AttachActor(SpawnedWeapon, FirstPersonMesh);
}

void AMain3DCharacter::Shoot(const FInputActionValue& Value)
{
	if(!IsValid(SpawnedWeapon)) return;
	
	USkeletalMeshComponent* WeaponMesh = SpawnedWeapon->FindComponentByClass<USkeletalMeshComponent>();
	
	if(!IsValid(WeaponMesh)) return;

	FVector Start = WeaponMesh->GetSocketTransform(FName("Muzzle_Front")).GetLocation();
	
	FVector End = Start + (SpawnedWeapon->GetActorForwardVector() * 50000.f);
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.Owner = this;
	
	AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Start, End.Rotation(), SpawnParameters);
	if(!IsValid(SpawnedProjectile)) return;
	SpawnedProjectile->SetDamage(50.f);
}

void AMain3DCharacter::Sprint(const FInputActionValue& Value, bool bSprint)
{
	if(!IsValid(GetCharacterMovement())) return;

	GetCharacterMovement()->MaxWalkSpeed = bSprint ? 900.f : 600.f;

	UE_LOG(LogTemp, Warning, TEXT("Sprint: %d"), bSprint)
}

