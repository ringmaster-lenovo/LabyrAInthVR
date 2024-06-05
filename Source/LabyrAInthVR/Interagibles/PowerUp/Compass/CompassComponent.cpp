#include "CompassComponent.h"

#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/VRGameMode.h"
#include "Particles/ParticleSystemComponent.h"

UCompassComponent::UCompassComponent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UCompassComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void UCompassComponent::TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	if (!IsValid(CompassInstance)) return;
	FVector ToPortal = (EndPortalPosition - GetOwner()->GetActorLocation()).GetSafeNormal() * 500.f;
	CompassInstance->SetWorldLocation(GetOwner()->GetActorLocation());
	CompassInstance->SetWorldRotation(ToPortal.Rotation());
}

void UCompassComponent::Trigger(UParticleSystem* CompassEffect)
{
	ClearCompassEffect();

	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!IsValid(GameMode))
	{
		return;
	}

	GameMode->GetSceneController()->GetEndPortalPositionAndRotation(EndPortalPosition,EndPortalRotation);
	FVector ToPortal = (EndPortalPosition - GetOwner()->GetActorLocation()).GetSafeNormal() * 500.f;

	if (!IsValid(CompassEffect)) return;

	CompassInstance = UGameplayStatics::SpawnEmitterAtLocation(this, CompassEffect, GetOwner()->GetActorLocation(),
															   ToPortal.Rotation());
	
}

void UCompassComponent::ClearCompassEffect()
{
	// if(!IsValid(CompassInstance)) return;
	// CompassInstance->Deactivate();
	if (IsValid(CompassInstance))
	{
		CompassInstance->Deactivate();
	}
}
