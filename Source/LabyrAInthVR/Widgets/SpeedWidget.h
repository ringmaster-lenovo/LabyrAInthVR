#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeedWidget.generated.h"

class AWidgetContainer;
/**
 * 
 */
UCLASS()
class USpeedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
};
