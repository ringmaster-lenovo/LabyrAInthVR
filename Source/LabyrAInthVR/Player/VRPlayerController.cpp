// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayerController.h"

#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Player_Log);

FString AVRPlayerController::TeleportPlayer(const FVector& Position, const FRotator& Rotation) const
{
	if (GetCharacter()->TeleportTo(Position, Rotation))
	{
		return "";
	}
	return "Cannot teleport player, game cannot start";
}
