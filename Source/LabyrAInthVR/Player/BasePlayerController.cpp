// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"

#include "PlayerStatistics.h"
#include "VRMainCharacter.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Player_Log);

void ABasePlayerController::BeginPlay()
{

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

	PlayerStatistics->ResetStats();
}

FString ABasePlayerController::TeleportPlayer(const FVector& Position, const FRotator& Rotation, bool InGamePassed) 
{
	if (MainCharacter->TeleportTo(Position, Rotation))
	{
		if (InGamePassed)
		{
			InGame = InGamePassed;
			UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();
			if (!IsValid(PlayerStatistics)) return "Cannot start level timer, PlayerStatistics ref is not valid";
			PlayerStatistics->StartLevelTimer();
			if(const ULocalPlayer* LocalPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
			{
				if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem =
					ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
				}
			}
		}
		else
		{
			
			UPlayerStatistics* PlayerStatistics = MainCharacter->GetPlayerStatistics();
			if (!IsValid(PlayerStatistics)) return "Cannot stop level timer, PlayerStatistics ref is not valid";
			PlayerStatistics->StopLevelTimer();
			if(const ULocalPlayer* LocalPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
			{
				if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem =
					ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					EnhancedInputLocalPlayerSubsystem->RemoveMappingContext(InputMappingContext);
				}
			}
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
				VRCharacter->SpawnPointer();
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

