// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeadWidget.generated.h"

class AWidgetContainer;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API UDeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	AWidgetContainer* WidgetContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* RestartButton;

	UFUNCTION()
	void OnRestartClicked();
	
};
