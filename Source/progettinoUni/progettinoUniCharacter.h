// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "progettinoUniCharacter.generated.h"

UCLASS(Blueprintable)
class AprogettinoUniCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AprogettinoUniCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
	int32 CoinsCollected = 0;

	UPROPERTY()
	int32 GemsCollected = 0;

	UPROPERTY()
	int32 TotalScore = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetCoinsCollected() const { return CoinsCollected; }

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetGemsCollected() const { return GemsCollected; }

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetTotalScore();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddCoinsCollected() { CoinsCollected += 1; }

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddGemsCollected() { GemsCollected += 1; }
};

