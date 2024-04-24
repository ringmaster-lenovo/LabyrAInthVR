// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetContainer.h"
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

public:
	virtual void Tick(float DeltaTime) override;
	
	bool bIsInVR = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<AWidgetContainer> WidgetContainerClass;

	UPROPERTY()
	AWidgetContainer* WidgetContainer = nullptr;

	DECLARE_MULTICAST_DELEGATE(FNewGameEvent);
	FNewGameEvent OnNewGameButtonClicked;

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
	void ShowMainMenu();

	UFUNCTION(Category = "Widgets")
	void ShowLoadingScreen();

	UFUNCTION(Category = "Widgets")
	void ShowGameUI();

	UFUNCTION(Category = "Widgets")
	void ShowWinScreen();

	UFUNCTION(Category = "Widgets")
	void ShowLoseScreen();

	UFUNCTION(Category = "Widgets")
	void NewGameButtonClicked();
	
};
