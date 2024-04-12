// Fill out your copyright notice in the Description page of Project Settings.


#include "VRMainCharacter.h"


#include "Engine/World.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Delegates/DelegateSignatureImpl.inl"


// This is the main character class for the VR game mode. It handles the VR camera, the VR controllers, and the VR movement.
AVRMainCharacter::AVRMainCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
 	VRRoot->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (!World) { return; }

	if (BlinkerFlag && BlinkerMaterialBase != nullptr)
	{
		BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
		PostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);
	}

}

// Called every frame
void AVRMainCharacter::Tick(float const DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (BlinkerFlag) UpdateBlinkerRadius();
}

// Called to bind functionality to input
void AVRMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Super::SetupPlayerInputComponent(PlayerInputComponent);
	//
	// PlayerInputComponent->BindAxis("ControllersForward", this, &AVRMainCharacter::MoveForwardControllers);
	// PlayerInputComponent->BindAxis("KeyboardForward", this, &AVRMainCharacter::MoveForwardKeyboard);
	// PlayerInputComponent->BindAxis("ControllersHorizontal", this, &AVRMainCharacter::MoveRightControllers);
	// PlayerInputComponent->BindAxis("KeyboardHorizontal", this, &AVRMainCharacter::MoveRightKeyboard);
	// PlayerInputComponent->BindAxis("VRRootHeight", this, &AVRMainCharacter::ChangeVRRootHeight);
	// PlayerInputComponent->BindAxis("RotateCameraOnYawAxis", this, &AVRMainCharacter::RotateCameraOnYawAxis);
	// PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Pressed, this, &AVRMainCharacter::RightTriggerPressed);
	// PlayerInputComponent->BindAction(TEXT("RightTrigger"), EInputEvent::IE_Released, this, &AVRMainCharacter::RightTriggerReleased);
	// PlayerInputComponent->BindAction(TEXT("LeftTrigger"), EInputEvent::IE_Pressed, this, &AVRMainCharacter::LeftTriggerPressed);
	// PlayerInputComponent->BindAction(TEXT("LeftTrigger"), EInputEvent::IE_Released, this, &AVRMainCharacter::LeftTriggerReleased);
	// PlayerInputComponent->BindAction(TEXT("RightBPressed"), EInputEvent::IE_Pressed, this, &AVRMainCharacter::RightBPressed);
	// PlayerInputComponent->BindAction(TEXT("LeftYPressed"), EInputEvent::IE_Pressed, this, &AVRMainCharacter::LeftYPressed);
	// PlayerInputComponent->BindAction(TEXT("LeftXPressed"), EInputEvent::IE_Pressed, this, &AVRMainCharacter::LeftXPressed);
	// PlayerInputComponent->BindAction("LeftGrip", IE_Pressed, this, &AVRMainCharacter::GripLeft);
	// PlayerInputComponent->BindAction("LeftGrip", IE_Released, this, &AVRMainCharacter::ReleaseLeft);
	// PlayerInputComponent->BindAction("RightGrip", IE_Pressed, this, &AVRMainCharacter::GripRight);
	// PlayerInputComponent->BindAction("RightGrip", IE_Released, this, &AVRMainCharacter::ReleaseRight);
	// PlayerInputComponent->BindAction("ResetVRRoot", IE_Pressed, this, &AVRMainCharacter::ResetVRRootPosition);
	
}

// usages: -Metarace: used to go to the next PlayerSpectatorPosition
void AVRMainCharacter::RightBPressed()
{
	if (!bEnableInputs) { return; }
	RightBPressedEvent.Broadcast();
}

// usages: -Metarace: used to go to the previous PlayerSpectatorPosition
void AVRMainCharacter::LeftYPressed()
{
	if (!bEnableInputs) { return; }
	LeftYPressedEvent.Broadcast();
}

// usages: -Metarace: change the VR position from spectator to jockey and vice versa
// this should always be executable
void AVRMainCharacter::LeftXPressed()
{
	LeftXPressedEvent.Broadcast();
}


// A function that rotate the VRRoot of the player along the yaw axis, but does not execute continuously when the button is held down
// it waits a while before it can be used again
void AVRMainCharacter::RotateCameraOnYawAxis(float const Value)
{
	if (!bEnableInputs) { return; }
	
	// Check if the cooldown timer has expired
	if (GetWorld()->GetTimeSeconds() > YawCooldownTime)
	{
		if (Value > 0.25f)  // a small dead-zone to prevent accidental rotation
		{
			// Calculate the new rotation of the camera
			FRotator NewRotation = VRRoot->GetComponentRotation();
			NewRotation.Yaw += 45.0f;  // 45° is the angle of rotation
			
			// Set the new rotation of the camera
			VRRoot->SetWorldRotation(NewRotation);
			
			YawCooldownTime = GetWorld()->GetTimeSeconds() + YawCooldownStartTime;
		}
		else if (Value < -0.25f)
		{
			// Calculate the new rotation of the camera
			FRotator NewRotation = VRRoot->GetComponentRotation();
			NewRotation.Yaw -= 45.0f;
			
			// Set the new rotation of the camera
			VRRoot->SetWorldRotation(NewRotation);
			
			YawCooldownTime = GetWorld()->GetTimeSeconds() + YawCooldownStartTime;
		}
	}
}

// updates the blinker radius based on the speed of the player
void AVRMainCharacter::UpdateBlinkerRadius() const
{
	if (RadiusVsVelocity == nullptr) return;

	const float Speed = GetVelocity().Size();
	const float Radius = RadiusVsVelocity->GetFloatValue(Speed);
	
	BlinkerMaterialInstance->SetScalarParameterValue(TEXT("BlinkerRadius"), Radius);

}


// toggle the visibility of the blinker
inline void AVRMainCharacter::SetBlinkerFlag(const bool Flag)
{
	BlinkerFlag = Flag;
}

// set the initial position of the VRRoot
// WORK IN PROGRESS
void AVRMainCharacter::ResetVRRootPosition()
{
	// chatgpt & copilot response on all the possible HMD names
	// Oculus Rift: "OculusHMD"
	// Oculus Quest: "OculusQuest"
	// Oculus Quest 2: "Oculus Quest 2"
	// HTC Vive: "SteamVR"
	// Valve Index: "Valve Index HMD"
	// Windows Mixed Reality: "Windows Mixed Reality HMD"
	// PlayStation VR: "PSVR"
	// Samsung Odyssey: "Samsung Odyssey HMD"

	// for debugging purposes
	// VRRootZHeight = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(this).GetLocation().Z;
	// UE_LOG(LogVR, Display, TEXT("VRRootZHeight: %f"), VRRootZHeight);
	// Camera->SetWorldLocation(FVector(0.0f, 0.0f, VRRootZHeight));

	// set the tracking origin to the eye level
	// UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	// AddLocalOffsetToVRRoot(FVector(0, 0, VRRootZHeightOffset_Standing));
	
	// if (HMDName == "Oculus Quest 2" || HMDName == "Oculus Quest" || HMDName == "Oculus Rift")
	// {
	// 	// FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	// 	// UE_LOG(LogVR, Warning, TEXT("Camera Offset: %s"), *NewCameraOffset.ToString());
	// 	// NewCameraOffset.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 	// NewCameraOffset.Z += VRRootZHeight;
	// 	// AddActorWorldOffset(NewCameraOffset);
	// 	// VRRoot->AddWorldOffset(-NewCameraOffset);
	// 	UE_LOG(LogVR, Display, TEXT("Oculus Quest 2, Oculus Quest, Oculus Rift"));
	// 	FVector HMDLocation;
	// 	FRotator HMDRotation;
	// 	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
	// 	HMDLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 	VRRoot->SetWorldLocation(HMDLocation);
	// 	VRRoot->SetWorldRotation(HMDRotation);
	// }
	// else if (HMDName == "Windows Mixed Reality")
	// {
	// 	UE_LOG(LogVR, Display, TEXT("I AM HERE Windows Mixed Reality"));
	// 	FVector HMDLocation;
	// 	FRotator HMDRotation;
	// 	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
	// 	HMDLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 	VRRoot->SetWorldLocation(HMDLocation);
	// 	VRRoot->SetWorldRotation(HMDRotation);
	// }
	// else
	// {
	// 	UE_LOG(LogVR, Display, TEXT("PSVR, Samsung Odyssey HMD, SteamVR, Valve Index HMD"));
	// 	FVector HMDLocation;
	// 	FRotator HMDRotation;
	// 	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
	// 	HMDLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 	VRRoot->SetWorldLocation(HMDLocation);
	// 	VRRoot->SetWorldRotation(HMDRotation);
	// }
}