// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "LobbyWidget.generated.h"

class AWidgetController;
// class AWidgetContainer;
/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// UPROPERTY()
	// AWidgetContainer* WidgetContainer;

	UPROPERTY()
	AWidgetController* WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WelcomeText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* LogoutButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* NewGameButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* LoadGameButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* PlayDemoButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* SettingsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUserWidget* QuitButton;
	

	UFUNCTION()
	void OnStartNewGameButtonClicked();

	UFUNCTION()
	void OnPlayDemoButtonClicked();

	UFUNCTION()
	void OnReplayContinueButtonClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnLogoutClicked();

	UFUNCTION()
	void SetFocusToButton();
	
};
