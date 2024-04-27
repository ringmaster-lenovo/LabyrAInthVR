// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetContainer.h"
#include "LobbyWidget.h"
#include "PromptingWidget.h"
#include "MenuWidget.h"
#include "SettingsWidget.h"
#include "LoadingWidget.h"
#include "WinWidget.h"
#include "LoseWidget.h"
#include "StatisticsWidget.h"
#include "WidgetController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrAInthVR_Widget_Log, Display, All);

UCLASS()
class LABYRAINTHVR_API AWidgetController : public AActor
{
	GENERATED_BODY()

public:
	AWidgetController();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Category = "Widgets")
	void RemoveAllWidgets(UObject* WorldContextObject);

public:
	virtual void Tick(float DeltaTime) override;
	
	bool bIsInVR = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<AWidgetContainer> WidgetContainerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UPromptingWidget> PromptingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UMenuWidget> MenuWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UStatisticsWidget> StatisticsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UWinWidget> WinWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<ULoseWidget> LoseWidgetClass;

	UPROPERTY()
	ULobbyWidget* LobbyWidget = nullptr;

	UPROPERTY()
	UPromptingWidget* PromptingWidget = nullptr;

	UPROPERTY()
	UMenuWidget* MenuWidget = nullptr;
	
	UPROPERTY()
	USettingsWidget* SettingsWidget = nullptr;

	UPROPERTY()
	UStatisticsWidget* StatisticsWidget = nullptr;

	UPROPERTY()
	ULoadingWidget* LoadingWidget = nullptr;

	UPROPERTY()
	UWinWidget* WinWidget = nullptr;

	UPROPERTY()
	ULoseWidget* LoseWidget = nullptr;

	UPROPERTY()
	AWidgetContainer* WidgetContainer = nullptr;

	DECLARE_MULTICAST_DELEGATE(FPlayGameEvent);
	FPlayGameEvent OnPlayGameButtonClicked;

	DECLARE_MULTICAST_DELEGATE(FRankingsEvent);
	FRankingsEvent OnRankingsButtonClicked;

	DECLARE_MULTICAST_DELEGATE(FQuitGameEvent);
	FQuitGameEvent OnQuitGameButtonClicked;

	DECLARE_MULTICAST_DELEGATE(FReturnToMainMenuEvent);
	FReturnToMainMenuEvent OnReturnToMainMenuEvent;

	DECLARE_MULTICAST_DELEGATE(FRestartLevelEvent);
	FRestartLevelEvent OnRestartLevelEvent;

	DECLARE_MULTICAST_DELEGATE(FPauseEvent);
	FPauseEvent OnPauseEvent;

	DECLARE_MULTICAST_DELEGATE(FResumeGameEvent);
	FResumeGameEvent OnResumeGameEvent;

	DECLARE_MULTICAST_DELEGATE(FWidgetsErrorEvent);
	FWidgetsErrorEvent OnWidgetSError;

	UFUNCTION(Category = "Widgets")
	void ShowLobbyUI();

	UFUNCTION(Category = "Widgets")
	void ShowMainMenu();

	UFUNCTION(Category = "Widgets")
	void ShowPromptingWidget();

	UFUNCTION(Category = "Widgets")
	void ShowLoadingScreen();

	UFUNCTION(Category = "Widgets")
	void ShowGameUI();

	UFUNCTION(Category = "Widgets")
	void ShowWinScreen(int32 TimeOnLevel);

	UFUNCTION(Category = "Widgets")
	void ShowLoseScreen();

	UFUNCTION(Category = "Widgets")
	void MainMenuButtonClicked();

	UFUNCTION(Category = "Widgets")
	void RestartButtonClicked();

	UFUNCTION(Category = "Widgets")
	void StartNewGameButtonClicked() const;

	UFUNCTION(Category = "Widgets")
	void ReplayContinueButtonClicked() const;

	UFUNCTION(Category = "Widgets")
	void NextLevelButtonClicked() const;

	UFUNCTION(Category = "Widgets")
	void RankingsButtonClicked() const;

	UFUNCTION(Category = "Widgets")
	void SettingsButtonClicked();

	UFUNCTION(Category = "Widgets")
	void QuitButtonClicked() const;

	UFUNCTION(Category = "Widgets")
	void OnPauseGamePressed();

	UFUNCTION(Category = "Widgets")
	void SendButtonClicked();
};
