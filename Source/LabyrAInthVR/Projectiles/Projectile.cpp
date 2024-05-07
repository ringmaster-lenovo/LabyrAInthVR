#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "LabyrAInthVR/Enemy/RangedEnemy.h"
#include "LabyrAInthVR/Interagibles/PowerUp.h"
#include "LabyrAInthVR/Pickups/BasePickup.h"
#include "LabyrAInthVR/Scene/ProceduralSplineWall.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Projectiles_Log);

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetGenerateOverlapEvents(true);
	SetRootComponent(CollisionBox);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	if (ProjectileTracer == nullptr) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		ProjectileTracer,
		CollisionBox,
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		false);
}

void AProjectile::Destroyed()
{
	Super::Destroyed();
	if (ImpactParticle == nullptr) return;

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, GetActorLocation());
}

void AProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile has impacted with: %s"), *OtherActor->GetName())
	
	if (OtherActor->IsA<AProceduralSplineWall>())
	{
		Destroy();
		PlaySound(SurfaceHit);
		return;
	}
	
	if (!IsValid(OtherActor) || OtherActor == GetOwner() || OtherActor->IsA<APowerUp>() || OtherActor->IsA<
		ABasePickup>() || (OtherActor->IsA<AMainCharacter>() && !GetOwner()->IsA<ARangedEnemy>()))
	{
		return;
	}

	if (!OtherActor->Implements<UDamageableActor>())
	{
		PlaySound(SurfaceHit);
		Destroy();
		return;
	}

	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), this,
	                              UDamageType::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("Playing sound"))
	PlaySound(BodyHit);
	Destroy();
}

void AProjectile::PlaySound(USoundCue* SoundToPlay)
{
	if(SoundToPlay == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("Playing sound"))
	UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
}
