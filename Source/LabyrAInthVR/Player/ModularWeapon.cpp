#include "ModularWeapon.h"

#include "LabyrAInthVR/Pickups/ModularWeaponPart.h"

AModularWeapon::AModularWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(SceneComponent);
	
	HandGuard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandGuard"));
	HandGuard->SetupAttachment(RootComponent);
	
	Stock = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Stock"));
	Stock->SetupAttachment(HandGuard);
	
	Scope = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scope"));
	Scope->SetupAttachment(HandGuard);
	
	Ammo = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ammo"));
	Ammo->SetupAttachment(HandGuard);
	
	Barrel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Barrel"));
	Barrel->SetupAttachment(HandGuard);
	
	Magazine = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Magazine"));
	Magazine->SetupAttachment(HandGuard);
	
	Trigger = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(HandGuard);
}

void AModularWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AModularWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AModularWeapon::AttachModularPart(AModularWeaponPart* ModularWeaponPart)
{
	
	if(!IsValid(ModularWeaponPart)) return;
	USkeletalMesh* ModularSkeletalMesh = ModularWeaponPart->GetSkeletalMeshPart();
	if(!IsValid(ModularSkeletalMesh)) return;
	
	switch(ModularWeaponPart->GetWeaponPart())
	{
	case Ewp_HandGuard:
		HandGuard->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	case Ewp_Stock:
		Stock->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	case Ewp_Scope:
		Scope->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	case Ewp_Ammo:
		Ammo->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	case Ewp_Barrel:
		Barrel->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	case Ewp_Magazine:
		Magazine->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	case Ewp_Trigger:
		Trigger->SetSkeletalMesh(ModularSkeletalMesh);
		break;
	default: ;
	}
}

