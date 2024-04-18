// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "VRMainCharacter.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Player_Log);

FString ABasePlayerController::TeleportPlayer(const FVector& Position, const FRotator& Rotation) const
{
	if (GetCharacter()->TeleportTo(Position, Rotation))
	{
		AVRMainCharacter* VRCharacter = Cast<AVRMainCharacter>(GetCharacter());
		if (VRCharacter != nullptr)
		{
			VRCharacter->IsInLobby = false;
			VRCharacter->StartTimer();
		}
		return "";
	}
	UE_LOG(LabyrAInthVR_Player_Log, Error, TEXT("Cannot teleport player, game cannot start"));
	return "Cannot teleport player, game cannot start";
}

void ABasePlayerController::CollidedWithEndPortal()
{
	OnCollisionWithEndPortal.Broadcast();
}
