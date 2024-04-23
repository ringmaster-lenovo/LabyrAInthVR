// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "LobbyWidget.h"
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

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* Widget;

	UFUNCTION(BlueprintCallable)
	FString ShowWidget(TSubclassOf<UUserWidget> WidgetClass);

	UPROPERTY(BlueprintReadOnly)
	bool bIsInVR = true;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
