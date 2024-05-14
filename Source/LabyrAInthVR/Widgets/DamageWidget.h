#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LabyrAInthVR/Player/MainCharacter.h"
#include "DamageWidget.generated.h"

UCLASS()
class LABYRAINTHVR_API UDamageWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	AMainCharacter* MainCharacter;
};
