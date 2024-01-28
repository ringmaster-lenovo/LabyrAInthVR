// Fill out your copyright notice in the Description page of Project Settings.


#include "GenMapWidget.h"

#include "WidgetController.h"
#include "Components/Button.h"
#include "../ProceduralMap.h"
#include "Components/SpinBox.h"
#include "Kismet/GameplayStatics.h"

void UGenMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GeneraButton)
	{
		GeneraButton->OnClicked.AddUniqueDynamic(this, &UGenMapWidget::OnGeneraButtonClicked);
	}
}

void UGenMapWidget::OnGeneraButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("GeneraButtonClicked"));
	// Replace AYourActorClass with the actual class of your actor
	const TSubclassOf<AProceduralMap> AProceduralMapClass = AProceduralMap::StaticClass();

	// Use the actor as needed
	if (AProceduralMap* ProceduralMap = Cast<AProceduralMap>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralMapClass)))
	{
		if (FloorLengthAmount && FloorWidthAmount)
		{
			ProceduralMap->SetFloorLength(FloorLengthAmount->GetValue());
			ProceduralMap->SetFloorWidth(FloorWidthAmount->GetValue());
			ProceduralMap->ProgrammaticallyPlaceFloor();
		}
		if (ArchwayAmount)
		{
			ProceduralMap->SetArchways(ArchwayAmount->GetValue());
		}
		if (PillarAmount)
		{
			ProceduralMap->SetPillars(PillarAmount->GetValue());
		}
		if (MovableBlockAmount)
		{
			ProceduralMap->SetMovableBlocks(MovableBlockAmount->GetValue());
		}
		if (CoinAmount)
		{
			ProceduralMap->SetCoins(CoinAmount->GetValue());
		}
		if (GemAmount)
		{
			ProceduralMap->SetGems(GemAmount->GetValue());
		}
		ProceduralMap->SpawnObjects();
	}
	
	WidgetController->ShowStartUI();
}
