// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROGETTINOUNI_API UEndWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* RestartButton;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Result;

	UPROPERTY()
	class AWidgetController* WidgetController = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWidgetController> BP_WidgetController;

	virtual void NativeConstruct() override;

	/*UFUNCTION()
	void OnRestartClicked();*/
	
};
