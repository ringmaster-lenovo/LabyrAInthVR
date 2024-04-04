#include "MockedCharacter.h"

AMockedCharacter::AMockedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMockedCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMockedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMockedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

