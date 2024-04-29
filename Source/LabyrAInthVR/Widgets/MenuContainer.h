// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MenuContainer.generated.h"


//DECLARE_DELEGATE_OneParam(FOnWantToSendMessage, const FString&);
UCLASS()

class LABYRAINTHVR_API AMenuContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMenuContainer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CloseMenu();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
