// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "LobbyWidget.h"
#include "SettingsWidget.h"
#include "WidgetContainer.generated.h"

//DECLARE_DELEGATE_OneParam(FOnWantToSendMessage, const FString&);
UCLASS()

class LABYRAINTHVR_API AWidgetContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWidgetContainer();

	/*FOnWantToSendMessage OnWantToSendMessage;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<USettingsWidget> SettingsWidgetClass;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	// TSubclassOf<UVRButtonsWidget> ButtonsWidgetClass;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	// TSubclassOf<UVRCountdownWidget> CountdownWidgetClass;
	//
	UPROPERTY()
	ULobbyWidget* LobbyWidget = nullptr;
	//
	UPROPERTY()
	USettingsWidget* SettingsWidget = nullptr;
	//
	// UPROPERTY()
	// UVRButtonsWidget* VRButtonsWidget = nullptr;
	//
	// UPROPERTY()
	// UVRCountdownWidget* VRCountdownWidget = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* Widget;

	UFUNCTION(BlueprintCallable)
	void ShowRankings();
	UFUNCTION(BlueprintCallable)
	void ShowSettings();
	/*UFUNCTION()
	void SpawnVRChooseTableUI(FString TableName, FString TableState, FString NumberOfPlayers);

	UFUNCTION()
	void SpawnButtonsUI(FString Balance);*/


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	

};
