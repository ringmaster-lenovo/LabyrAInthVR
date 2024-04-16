#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LabyrAInthVR/MockedCharacter/MockedCharacter.h"
#include "LabyrAInthVR/Scene/ProceduralSplineWall.h"
#include "LabyrAInthVR/Player/MainCharacter.h"

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

	/*ProjectileTracerComponent = UGameplayStatics::SpawnEmitterAttached(
		ProjectileTracer,
		CollisionBox,
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition);*/

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
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == GetOwner()) return;

	if (OtherActor->IsA<AProceduralSplineWall>())
	{
		Destroy();
		return;
	}

	if (!OtherActor->Implements<UDamageableActor>()) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Projectile has impacted with: %s"), *OtherActor->GetName())
	AMainCharacter* Player = Cast<AMainCharacter> (OtherActor);
	if (!Player)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
	} else
	{
		Player->ReceiveDamage(Damage, this);
	}
	Destroy();
}

