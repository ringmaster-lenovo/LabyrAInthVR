#include "DamageWidget.h"
#include "StatisticsWidget.h"
#include "LabyrAInthVR/Player/PlayerStatistics.h"

void UDamageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UDamageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	MainCharacter = (!IsValid(MainCharacter)) ? Cast<AMainCharacter>(GetOwningPlayerPawn()) : MainCharacter;
    
	if (!IsValid(MainCharacter) || !IsValid(MainCharacter->GetPlayerStatistics()))
	{
		UE_LOG(LogTemp, Error, TEXT("Main character or Player statistics not valid from widget"))
		return;
	}
}
