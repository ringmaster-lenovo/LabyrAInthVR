// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StartMenuWidget.h"
#include "EndWidget.h"
#include "GenMapWidget.h"
#include "PauseMenuWidget.h"
#include "TimerWidget.h"
#include "GameFramework/Actor.h"
#include "../Character/3DPlayerController.h"
#include "WidgetController.generated.h"


UCLASS()
class LABYRAINTH_API AWidgetController : public AActor
{
	GENERATED_BODY()

private:
	FTimerHandle TimerHandle;

	void UpdateTimer() const {if (BP_TimerWidget) TimerWidget->UpdateTimer();}
	
public:	
	// Sets default values for this actor's properties
	AWidgetController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UGenMapWidget> BP_GenMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UStartMenuWidget> BP_StartWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UEndWidget> BP_EndWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UTimerWidget> BP_TimerWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UPauseMenuWidget> BP_PauseMenuWidget;

	UPROPERTY()
	A3DPlayerController* PlayerController3D;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UGenMapWidget* GenMapWidget;
	
	UPROPERTY()
	UStartMenuWidget* StartMenuWidget;

	UFUNCTION()
	UStartMenuWidget* GetStartMenuWidget() const;

	UPROPERTY()
	UEndWidget* EndWidget;

	UPROPERTY()
	UTimerWidget* TimerWidget;

	UPROPERTY()
	UPauseMenuWidget* PauseMenuWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetAvailableTimeAmount(const int32& AvailableTimeAmount) const;

	void ShowGenMapUI() const;
	
	void ShowStartUI() const;

	void StartGame() const;

	void PauseGame() const;

	void ResumeGame() const;

	void QuitGame() const;

	void ShowEndUI() const;

	static void RestartGame();
	
};