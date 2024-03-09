#pragma once

#include "CoreMinimal.h"
#include "LabyrinthParser.h"
#include "GameFramework/Actor.h"
#include "ProceduralSplineWall.generated.h"

class USplineComponent;

UCLASS()
class LABYRAINTHVR_API AProceduralSplineWall : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralSplineWall();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SplineMesh")
	void UpdateSplineMesh();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SplineMesh")
	void AddPillarAtStart(bool bAdd);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SplineMesh")
	void AddPillarAtEnd(bool bAdd);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SplineMesh")
	void AddWallTop(bool bAdd);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SplineMesh")
	void EditWallLook(uint8 WallType);
	
	void AddSplinePoint(FVector& Location);
	void AddSplinePointAtIndex(FVector& Location, int32 Index);
	void ClearSplinePoints();
	bool GetBackwardNeighbor(FVector& Location, ETravellingDirection TravellingDirection);

	FORCEINLINE USplineComponent* GetSplineComponent() const { return SplineComponent; }
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USceneComponent* DefaultRoot;

	FTimerHandle CreatePointsTimerHandle;

	UFUNCTION()
	void CreatePointsTimerFinished();
};
