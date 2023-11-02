// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROGETTINOUNI_API UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY()
	class AWidgetController* WidgetController = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWidgetController> BP_3DWidgetController;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartClicked();
	
};
