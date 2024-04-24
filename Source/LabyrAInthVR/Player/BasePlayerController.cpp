﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "VRMainCharacter.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Player_Log);

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

FString ABasePlayerController::GetPlayerName() const
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot get player name, no character is controlled by the player controller"));
		return "";
	}
	return MainCharacter->GetPlayerName();
}

void ABasePlayerController::SetPlayerName(const FString& Name)
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot set player name, no character is controlled by the player controller"));
		return;
	}
	MainCharacter->SetPlayerName(Name);
}

int32 ABasePlayerController::GetPlayerTimeOnCurrentLevel() const
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot set player name, no character is controlled by the player controller"));
		return -1;
	}
	return MainCharacter->GetTimeOnCurrentLevel();
}

void ABasePlayerController::ResetPlayerStats()
{
	if (MainCharacter == nullptr)
	{
		UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot reset player stats, no character is controlled by the player controller"));
		return;
	}
	MainCharacter->ResetStats();
}

FString ABasePlayerController::TeleportPlayer(const FVector& Position, const FRotator& Rotation, const bool InGame) const
{
	if (MainCharacter->TeleportTo(Position, Rotation))
	{
		if (InGame)
		{
			MainCharacter->StartLevelTimer();
		}
		else
		{
			MainCharacter->StopAllTimers();
		}
		if (AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(MainCharacter); VRCharacter != nullptr)
		{
			if (InGame)
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

void ABasePlayerController::PlayerHasDied() const
{
	OnPLayerDeath.Broadcast();
}

