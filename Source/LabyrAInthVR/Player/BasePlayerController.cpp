// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"

#include "PlayerStatistics.h"
#include "VRMainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LabyrAInthVR/Widgets/MenuContainer.h"
#include "EnhancedInputSubsystems.h"
#include "Main3DCharacter.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Player_Log);

void ABasePlayerController::BeginPlay()
{
	// I left this for testing in WeaponTestingMap
	 if(const ULocalPlayer* LocalPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
	 {
	 	if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem =
	 		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	 	{
	 		EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
	 	}
	 }
}

void ABasePlayerController::SetControlledCharacter(AMainCharacter* AMainCharacter)
{
	if (AMainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot set a null character as controlled character"));
		return;
	}
	if (AMainCharacter != GetCharacter())
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot set a character that is not the one controlled by the player controller"));
		return;
	}
	MainCharacter = AMainCharacter;
}

AMainCharacter* ABasePlayerController::GetControlledCharacter() const
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot get controlled character, no character is controlled by the player controller"));
	}
	return MainCharacter;
}

void ABasePlayerController::SetLevelTimer(const float Time) const
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot set player name, no character is controlled by the player controller"));
		return;
	}
	MainCharacter->GetPlayerStatistics()->SetLevelTimer(Time);
}

int32 ABasePlayerController::GetPlayerTimeOnCurrentLevel() const
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot set player name, no character is controlled by the player controller"));
		return -1;
	}
	
	UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();
	
	if (!IsValid(PlayerStatistics))
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot get player statistics"));
		return -1;
	}
	
	return PlayerStatistics->GetLevelTime();
}

void ABasePlayerController::ResetPlayerStats()
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot reset player stats, no character is controlled by the player controller"));
		return;
	}
	UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();

	if (!IsValid(PlayerStatistics))
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot reset player stats, PlayerStatistics ref is not valid"));
		return;
	}
	GetWorldTimerManager().ClearAllTimersForObject(PlayerStatistics);
	PlayerStatistics->ResetStats();
}

void ABasePlayerController::CloseVRHandMenu()
{
	if (AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(MainCharacter); VRCharacter != nullptr)
	{
		AMenuContainer* MenuContainer = Cast<AMenuContainer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuContainer::StaticClass()));
		if(MenuContainer)
		{
			MenuContainer->CloseMenu();
		}
	}
}

void ABasePlayerController::SpawnVRPointer()
{
	if (AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(MainCharacter); VRCharacter != nullptr)
	{
		VRCharacter->IsInLobby = true;
		VRCharacter->SpawnPointer();
	}
}

FString ABasePlayerController::TeleportPlayer(const FVector& Position, const FRotator& Rotation, const bool InGamePassed) 
{
	if (MainCharacter->TeleportTo(Position, Rotation))
	{
		if (InGamePassed)
		{
			InGame = InGamePassed;
			UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();
			if (!IsValid(PlayerStatistics)) return "Cannot start level timer, PlayerStatistics ref is not valid";
			PlayerStatistics->StartLevelTimer();
		}
		else
		{
			UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();
			if (!IsValid(PlayerStatistics)) return "Cannot stop level timer, PlayerStatistics ref is not valid";
			PlayerStatistics->StopLevelTimer();
			// MainCharacter->SetActorRotation(FRotator{0.f, 0.f, 0.f});  // TODO DOES NOT WORK
			GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &ThisClass::BlockMovementInLobby, 1.0f, false, .5f);
		}
		if (AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(MainCharacter); VRCharacter != nullptr)
		{
			if (InGamePassed)
			{
				VRCharacter->IsInLobby = false;
				VRCharacter->StopWidgetInteraction();
				VRCharacter->DestroyPointer();
				VRCharacter->DestroyPointerLeft();
			}
			else
			{
				VRCharacter->IsInLobby = true;
				CloseVRHandMenu();
				VRCharacter->SpawnPointer();
			}
		}
		else if (AMain3DCharacter* Main3DCharacter = Cast<AMain3DCharacter>(MainCharacter); Main3DCharacter != nullptr)
		{
			if (InGamePassed)
			{
				if (const ULocalPlayer* LocalPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
				{
					if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem =
						ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
					{
						EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
					}
				}
			}
			else
			{
				if (const ULocalPlayer* LocalPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
				{
					if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem =
						ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
					{
						EnhancedInputLocalPlayerSubsystem->RemoveMappingContext(InputMappingContext);
					}
				}
			}
		}
		return "";
	}
	UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot teleport player, unplayable game state"));
	return "Cannot teleport player, unplayable game state";
}

void ABasePlayerController::CollidedWithEndPortal() const
{
	OnCollisionWithEndPortal.Broadcast();
}

void ABasePlayerController::PlayerHasDied()
{
	NumOfDeaths++;
	OnPLayerDeath.Broadcast();
}

void ABasePlayerController::PlayerTimerWentOff()
{
	OnPLayerFinishedTimer.Broadcast();
}

void ABasePlayerController::BlockMovementInLobby()
{
	MainCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
	// stop timer for teleporting
	GetWorldTimerManager().ClearTimer(TeleportTimerHandle);
}

