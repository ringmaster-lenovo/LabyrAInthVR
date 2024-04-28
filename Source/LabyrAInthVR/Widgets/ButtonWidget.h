// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ButtonWidget.generated.h"

class AWidgetController;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API UButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FText ButtonText;
	
};
