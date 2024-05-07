// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap.h"
#include <Kismet/GameplayStatics.h>
#include "LabyrAInthVR/Enemy/RangedEnemy.h"
#include "LabyrAInthVR/Enemy/BaseEnemy.h"
#include "LabyrAInthVR/Enemy/MeleeEnemy.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "LabyrAInthVR/Player/PlayerStatistics.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Trap_Log);

// Sets default values
ATrap::ATrap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATrap::Freeze(int32 Time)
{
	if (Time > 0)
	{
		IFreezableActor::Freeze(Time);
		UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> Frozen for %d second"), *GetName(), Time)
		bIsFrozen = true;
		CustomTimeDilation = 0.f;
		ReceiveFreeze();
		GetWorldTimerManager().SetTimer(FreezingTimerHandle, this, &ThisClass::FreezeTimerFinished, Time, false);
	}
}

void ATrap::FreezeTimerFinished()
{
	/*GetMesh()->bNoSkeletonUpdate = false;
	UE_LOG(LogTemp, Warning, TEXT("Enemy state after: %s"), *UEnum::GetValueAsString(EnemyState))
	*/
	//AIController->MoveTo(MoveRequest);
	bIsFrozen = false;
	CustomTimeDilation = 1.f;
	ReceiveFreezeResume();
	UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> Resume after being frozen"), *GetName())
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*Utilities*/

bool isMainCharacter(AActor* Actor)
{
	return Actor->IsA<AMainCharacter>();
}

bool isEnemy(AActor* Actor)
{
	return Actor->IsA<ABaseEnemy>();
}

/*Private*/

void ATrap::DamagePlayer(AActor* Actor, float Damage)
{
	
	AMainCharacter* MainCharacter=Cast<AMainCharacter>(Actor);
	if (MainCharacter != nullptr && IsValid(MainCharacter->GetPlayerStatistics()))
	{
		UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();
		PlayerStatistics->ChangeStatFloat(Esm_Health, -Damage);
		UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> MainCharacter took %f damage"), *GetName(), Damage)
	}
}

void ATrap::DamageEnemy(AActor* Actor, float Damage)
{
	ARangedEnemy* RangedEnemy = Cast<ARangedEnemy>(Actor);
	if (RangedEnemy != nullptr)
	{
		UGameplayStatics::ApplyDamage(Actor, Damage, GetOwner()->GetInstigatorController(), this,
								  UDamageType::StaticClass());
		UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> %s took %f damage"), *GetName(), *Actor->GetName(), Damage)
	}
	else
	{
		AMeleeEnemy* MeleeEnemy = Cast<AMeleeEnemy>(Actor);
		if(MeleeEnemy!=nullptr)
		{
			UGameplayStatics::ApplyDamage(Actor, Damage, GetOwner()->GetInstigatorController(), this,
								  UDamageType::StaticClass());
			UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> %s took %f damage"), *GetName(), *Actor->GetName(), Damage)
		}
	}
}

/*Exposed function*/

void ATrap::OnEnterSensingArea(AActor* Actor)
{
	if(IsValid(Actor))
	{
		if(isMainCharacter(Actor))
		{
			bCharacterInSensingArea=true;
			UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> MainCharacter enter sensing area"), *GetName())
		}
	}
}

void ATrap::OnLeaveSensingArea(AActor* Actor)
{
	if(IsValid(Actor))
	{
		if(isMainCharacter(Actor))
		{
			bCharacterInSensingArea=false;
			UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> MainCharacter leave sensing area"), *GetName())
		}
	}
}

void ATrap::OnEnterDamageArea(AActor* Actor)
{
	if(Actor->IsValidLowLevel())
	{
		if(isMainCharacter(Actor) || isEnemy(Actor))
		{
			ActorInRange.AddUnique(Actor);
			UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> %s enter damage area"), *GetName(), *Actor->GetName())
		}
	}
}

void ATrap::OnLeaveDamageArea(AActor* Actor)
{
	if(Actor->IsValidLowLevel())
	{
		if(isMainCharacter(Actor) || isEnemy(Actor))
		{
			ActorInRange.Remove(Actor);
			UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> %s leave damage area"), *GetName(), *Actor->GetName())
		}
	}
}

bool ATrap::IsTrapWorking()
{
	return bCharacterInSensingArea && !bIsFrozen;
}

void ATrap::ApplyDamage(AActor* Actor, float Damage)
{
	if(IsValid(Actor) && Damage && (isMainCharacter(Actor) || isEnemy(Actor)) )
	{
		UGameplayStatics::ApplyDamage(Actor, Damage, GetInstigatorController() , this, UDamageType::StaticClass());
		UE_LOG(LabyrAInthVR_Trap_Log, Display, TEXT("%s -> %s took %f damage"), *GetName(), *Actor->GetName(), Damage)
	}
}

void ATrap::ApplyDamageToAll(float Damage)
{
	for(AActor* Actor : ActorInRange)
	{
		ApplyDamage(Actor, Damage);
	}
}
