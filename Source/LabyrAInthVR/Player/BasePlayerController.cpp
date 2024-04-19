// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "VRMainCharacter.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Player_Log);

FString ABasePlayerController::TeleportPlayer(const FVector& Position, const FRotator& Rotation, const bool InGame) const
{
	if (GetCharacter()->TeleportTo(Position, Rotation))
	{
		AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(GetCharacter());
		if (VRCharacter != nullptr)
		{
			if (InGame)
			{
				VRCharacter->IsInLobby = false;
				VRCharacter->StartTimer();
			}
			else
			{
				VRCharacter->IsInLobby = true;
			}
		}
		return "";
	}
	UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot teleport player, unplayable game state"));
	return "Cannot teleport player, unplayable game state";
}

void ABasePlayerController::CollidedWithEndPortal()
{
	OnCollisionWithEndPortal.Broadcast();
}
