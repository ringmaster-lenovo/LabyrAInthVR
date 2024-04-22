#include "BasePickup.h"

#include "Components/SphereComponent.h"
#include "LabyrAInthVR/Player/MainCharacter.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(StaticMeshComponent);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	SphereComponent->SetupAttachment(StaticMeshComponent);
}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

void ABasePickup::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Start overlap: %s"), *GetName());
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if(!IsValid(MainCharacter)) return;
	MainCharacter->SetOverlappedPickup(this);
}

void ABasePickup::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap: %s"), *GetName());
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if(!IsValid(MainCharacter)) return;
	MainCharacter->SetOverlappedPickup(nullptr);
}

void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

