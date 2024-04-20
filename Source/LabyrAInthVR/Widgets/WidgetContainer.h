// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "LobbyWidget.h"
#include "SettingsWidget.h"
#include "LoadingWidget.h"
#include "StatisticsWidget.h"
#include "WidgetContainer.generated.h"

class AWidgetController;

//DECLARE_DELEGATE_OneParam(FOnWantToSendMessage, const FString&);
UCLASS()

class LABYRAINTHVR_API AWidgetContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWidgetContainer();

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	AWidgetController* WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UStatisticsWidget> StatisticsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;
	
	UPROPERTY()
	ULobbyWidget* LobbyWidget = nullptr;
	
	UPROPERTY()
	USettingsWidget* SettingsWidget = nullptr;

	UPROPERTY()
	UStatisticsWidget* StatisticsWidget = nullptr;

	UPROPERTY()
	ULoadingWidget* LoadingWidget = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* Widget;

	UFUNCTION(BlueprintCallable)
	FString ShowMainMenuUI();

	UFUNCTION(BlueprintCallable)
	FString ShowSettings();
	
	UFUNCTION(BlueprintCallable)
	FString ShowLoadingUI();
	
	UFUNCTION(BlueprintCallable)
	FString ShowRankings();

	UFUNCTION(BlueprintCallable)
	FString HideMainMenuUI();

	UFUNCTION()
	void NewGameButtonClicked() const;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInVR = true;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
