// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralFloor.generated.h"

UCLASS()
class PROGETTINOUNI_API AProceduralFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	int32 FloorLength;

	int32 FloorWidth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FloorMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pillar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Pillar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pillar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Wall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pillar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_MovableBlocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pillar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BP_Gems;

	void ProcedurallySpawnObjects(UClass* ObjectClass, int32 NumberOfObjects = 1, bool bCheckCollision = false, float ZHeight = 0) const;

	bool CheckOverlap(const AActor* ActorToIgnore, const FVector& SpawnLocation, const FVector& BoxHalfExtent) const;
};
