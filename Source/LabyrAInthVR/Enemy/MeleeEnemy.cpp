#include "MeleeEnemy.h"

#include "Components/BoxComponent.h"

AMeleeEnemy::AMeleeEnemy()
{
	WeaponBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBoxComponent"));
	WeaponBoxComponent->SetupAttachment(GetMesh(), FName("weapon_r"));
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(WeaponBoxComponent)) return;

	WeaponBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
}

void AMeleeEnemy::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
	SetWeaponCollision(false);
}

void AMeleeEnemy::SetWeaponCollision(bool bEnabled)
{
	if (!IsValid(WeaponBoxComponent)) return;

	WeaponBoxComponent->SetCollisionEnabled(bEnabled
												? ECollisionEnabled::QueryAndPhysics
												: ECollisionEnabled::NoCollision);
}

void AMeleeEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										 const FHitResult& SweepResult)
{
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (!IsValid(HitCharacter) || OtherActor == this) return;
	UE_LOG(LogTemp, Warning, TEXT("Enemy attacked: %s"), *OtherActor->GetName())
}