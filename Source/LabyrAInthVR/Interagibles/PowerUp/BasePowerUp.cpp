#include "BasePowerUp.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Core/VRGameMode.h"
#include "LabyrAInthVR/Player/MainCharacter.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_PowerUp_Log);

ABasePowerUp::ABasePowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	SetRootComponent(NiagaraComponent);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SphereComponent->SetupAttachment(NiagaraComponent);
}

// Called when the game starts or when spawned
void ABasePowerUp::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
}

// Called every frame
void ABasePowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasePowerUp::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if PowerUp is overlapped by the player
	MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (!IsValid(MainCharacter)) return;
	
	UE_LOG(LabyrAInthVR_PowerUp_Log, Display, TEXT("%s: start overlap"), *GetName());
	if(IsValid(CollectSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation());
	}
	if(NiagaraEmitter!=nullptr && NiagaraEmitter->IsValid())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NiagaraEmitter, GetActorLocation());
	}
	
	/*PowerUp logic here*/
	PowerUp();
	
	AVRGameMode* GameMode = Cast<AVRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->GetSceneController()->RemoveFromSpawnManagerList(this);
	this->K2_DestroyActor();
}

void ABasePowerUp::PowerUp()
{
	
}
