// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMap.generated.h"

UCLASS()
class PROGETTINOUNI_API AProceduralMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	int32 FloorLength = 1000;  // floor length in cm in the X axis

	int32 FloorWidth = 1000;  // floor width in cm in the Y axis
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FloorMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Wall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Archway", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Archway;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pillar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Pillar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovableBlock", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_MovableBlocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Coin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Gem;

	UPROPERTY(VisibleAnywhere, Category = "Archway", meta = (AllowPrivateAccess = "true"))
	uint32 ArchwayAmount = 1;
	
	UPROPERTY(VisibleAnywhere, Category = "Pillar", meta = (AllowPrivateAccess = "true"))
	uint32 PillarAmount = 1;
	
	UPROPERTY(VisibleAnywhere, Category = "MovableBlock", meta = (AllowPrivateAccess = "true"))
	uint32 MovableBlockAmount = 1;

	UPROPERTY(VisibleAnywhere, Category = "Coin", meta = (AllowPrivateAccess = "true"))
	uint32 CoinAmount = 1;

	UPROPERTY(VisibleAnywhere, Category = "Gem", meta = (AllowPrivateAccess = "true"))
	uint32 GemAmount = 1;

	void ProcedurallySpawnObjects(UClass* ObjectClass, int32 NumberOfObjects = 1, bool bCheckCollision = false, float ZHeight = 0) const;

	bool IsActorColliding(const AActor* ActorToCheck, const FVector& ActorSpawnLocation, const FVector& ActorBoxHalfExtent) const;

public:
	FORCEINLINE int32 GetFloorLength() const {return FloorLength;}
	FORCEINLINE int32 GetFloorWidth() const {return FloorWidth;}
	void SetFloorLength(const int32 Length) {FloorLength = Length;}
	void SetFloorWidth(const int32 Width) {FloorWidth = Width;}
	
	UFUNCTION()
	void SetArchways(const uint32 Amount) {ArchwayAmount = Amount;}
	UFUNCTION()
	void SetPillars(const uint32 Amount) {PillarAmount = Amount;}
	UFUNCTION()
	void SetMovableBlocks(const uint32 Amount) {MovableBlockAmount = Amount;}
	UFUNCTION()
	void SetCoins(const uint32 Amount) {CoinAmount = Amount;}
	UFUNCTION()
	void SetGems(const uint32 Amount) {GemAmount = Amount;}
	
	void PlaceWalls();

	void ProgrammaticallyPlaceFloor();

	void SpawnObjects() const;

	void CleanObject(TSubclassOf<AActor> BP_Object) const;

};
