#include "MockedCharacter.h"

AMockedCharacter::AMockedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMockedCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
}

void AMockedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMockedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMockedCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character received damage by: %s"), *DamageCauser->GetName())
}

