#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlowWidget.generated.h"

class AWidgetContainer;
/**
 * 
 */
UCLASS()
class USlowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
};
