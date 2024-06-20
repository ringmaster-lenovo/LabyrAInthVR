#include "ModularWeaponPart.h"

#include "BasePickup.h"
#include "Components/SphereComponent.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "LabyrAInthVR/Player/ModularWeapon.h"

AModularWeaponPart::AModularWeaponPart()
{
	PrimaryActorTick.bCanEverTick = true;

	ModularPartStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(ModularPartStaticMesh);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	SphereComponent->SetupAttachment(ModularPartStaticMesh);
}

void AModularWeaponPart::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

void AModularWeaponPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AModularWeaponPart::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	UE_LOG(LabyrAInthVR_Pickups_Log, Display, TEXT("Start overlap: %s"), *GetName());
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetEquippedWeapon())) return;
	
	AModularWeapon* ModularWeapon = Cast<AModularWeapon>(MainCharacter->GetEquippedWeapon());
	
	if(!IsValid(ModularWeapon)) return;
	
	UE_LOG(LabyrAInthVR_Pickups_Log, Display, TEXT("The player has the modular weapon"));
	ModularWeapon->AttachModularPart(this);
	Destroy();
}

void AModularWeaponPart::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LabyrAInthVR_Pickups_Log, Display, TEXT("Start overlap: %s"), *GetName());
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (!IsValid(MainCharacter)) return;
}
