#include "3DMainCharacter.h"

#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"

// This is the main character class for the VR game mode. It handles the VR camera, the VR controllers, and the VR movement.
A3DMainCharacter::A3DMainCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void A3DMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World) { return; }
}

// Called every frame
void A3DMainCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void A3DMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}