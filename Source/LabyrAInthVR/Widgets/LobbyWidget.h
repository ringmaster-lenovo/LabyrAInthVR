// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "WidgetContainer.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class LABYRAINTHVR_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* NewGame;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* LoadGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Rankings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Quit;

	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	// TSubclassOf<AWidgetContainer>* BP_WidgetContainer;
 
	// UPROPERTY()
	// AWidgetContainer* WidgetContainer = nullptr;

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnLoadGameClicked();

	UFUNCTION()
	void OnRankingsClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitClicked();
	
};
