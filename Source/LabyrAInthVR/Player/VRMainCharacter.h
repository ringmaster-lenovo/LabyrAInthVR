// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
// #include "Poker3DVR/Metarace/VRMetaraceController.h"
#include "VRMainCharacter.generated.h"


UCLASS()
class LABYRAINTHVR_API AVRMainCharacter : public AMainCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsInLobby = true;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StopWidgetInteraction();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SpawnPointer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DestroyPointer();

	UFUNCTION(BlueprintImplementableEvent)
	void DestroyPointerLeft();
	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VRRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementSpeed", meta = (AllowPrivateAccess = "true"))
	double BaseMovementSpeed = 0.5;
	

	UPROPERTY(EditAnywhere, Category = "VR")
	class UCurveFloat* RadiusVsVelocity;

	UPROPERTY(EditAnywhere, Category = "VR")
	class UMaterialInterface* BlinkerMaterialBase;
	
	UPROPERTY()
	class UMaterialInstanceDynamic* BlinkerMaterialInstance;
	
	UPROPERTY(EditAnywhere, Category = "VR")
	class UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditAnywhere)
	bool BlinkerFlag = true;

	UPROPERTY(EditAnywhere, Category = "VR")
	float YawCooldownStartTime = 0.3f;
	
	UPROPERTY(EditAnywhere, Category = "VR")
	float YawCooldownTime = YawCooldownStartTime;

	

public:
	USceneComponent* GetVRRoot() const;
	void AddLocalOffsetToVRRoot(const FVector& Location) const;
	void AddLocalOffsetToVRRoot(const FVector& Location, const FRotator& Rotation) const;
	void SetVRRootRotation(const FRotator& Rotation) const;
	void SetBlinkerFlag(bool Flag);
	// bool GetHMDAndControllerInformation(FName& HMDName);

	UPROPERTY(EditAnywhere, Category = "VR")
	bool bEnableInputs = true;

	DECLARE_MULTICAST_DELEGATE(FRightBPressedEvent);
	FRightBPressedEvent RightBPressedEvent;

	DECLARE_MULTICAST_DELEGATE(FLeftYPressedEvent);
	FLeftYPressedEvent LeftYPressedEvent;

	DECLARE_MULTICAST_DELEGATE(FLeftXPressedEvent);
	FLeftXPressedEvent LeftXPressedEvent;

	UFUNCTION(BlueprintCallable)
	void RotateCameraOnYawAxis(float const Value);

private:

	void RightBPressed();
	void LeftYPressed();
	void LeftXPressed();

	void ChangeVRRootHeight(float const Value);

	void ResetVRRootPosition();

	void UpdateBlinkerRadius() const;

	
};
