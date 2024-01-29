// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenMapWidget.generated.h"

class USpinBox;
/**
 * 
 */
UCLASS()
class PROGETTINOUNI_API UGenMapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	class AWidgetController* WidgetController = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWidgetController> BP_WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* GeneraButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Time")
	USpinBox* AvailableTimeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* FloorWidthAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* FloorLengthAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* ArchwayAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* PillarAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* MovableBlockAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* CoinAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Map")
	USpinBox* GemAmount;

	UFUNCTION(BlueprintCallable)
	void OnGeneraButtonClicked();
	
};
