// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadLevelsWidget.generated.h"

class AWidgetController;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ULoadLevelsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	AWidgetController* WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* ScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* LevelsBox;
	
};
