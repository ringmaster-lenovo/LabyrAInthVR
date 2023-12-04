// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralFloor.h"

// Sets default values
AProceduralFloor::AProceduralFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComponent"));
	SetRootComponent(FloorMeshComponent);
}

// Called when the game starts or when spawned
void AProceduralFloor::BeginPlay()
{
	Super::BeginPlay();

	// Get the dimensions of the floor
	FloorLength = GetRootComponent()->GetRelativeTransform().GetScale3D().X * 100;
	UE_LOG(LogTemp, Warning, TEXT("FloorLength: %d"), FloorLength);
	FloorWidth = GetRootComponent()->GetRelativeTransform().GetScale3D().Y * 100;
	UE_LOG(LogTemp, Warning, TEXT("FloorWidth: %d"), FloorWidth);
	
	ProcedurallySpawnObjects(BP_Wall, 40);
	ProcedurallySpawnObjects(BP_Pillar, 30, true);
	ProcedurallySpawnObjects(BP_MovableBlocks, 20, true);
	ProcedurallySpawnObjects(BP_Gems, 10, true, 100);
}

// Called every frame
void AProceduralFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
 * @brief Procedurally spawn objects in the floor, takes into account to not spawn objects inside the walls on the edge of the floor,
 * it tries to spawn the objects in a random location and rotation, if bCheckCollision is true it checks if the object overlaps with other objects,
 * if it does it destroys the object and tries again, if it fails to spawn the object after 100 attempts it stops trying.
 * If the object is a wall it rotate the objects only in 4 directions (0, 45, 90, 135) and place them farther from the edges of the floor avoiding clipping with the edges's walls.
 * @param ObjectClass The class of the object to spawn
 * @param NumberOfObjects The number of objects to spawn, defaults to 1
 * @param bCheckCollision Whether to check for collision with other objects, defaults to false
 * @param ZHeight The height at which to spawn the objects, defaults to 0
 */
void AProceduralFloor::ProcedurallySpawnObjects(UClass* ObjectClass, const int32 NumberOfObjects, const bool bCheckCollision, const float ZHeight) const
{
	int32 SpawnedObjects = 0;
	constexpr int32 MaxAttempts = 100;
	int32 Attempts = 0;
	while (SpawnedObjects < NumberOfObjects && Attempts < MaxAttempts)
	{
		FVector SpawnLocation;
		FRotator SpawnRotation;
		constexpr int32 DistanceFromWalls = 100;  // thickness of the edge's walls
		if (ObjectClass == BP_Wall)
		{
			int32 RandomSide = FMath::RandRange(0, 3);  // only 4 rotations
			if (RandomSide == 0)
			{
				SpawnRotation = FRotator(0, 0, 0);
				SpawnLocation = FVector(FMath::RandRange(DistanceFromWalls, FloorLength - (4 * DistanceFromWalls)),
						FMath::RandRange(DistanceFromWalls, FloorWidth - DistanceFromWalls), ZHeight);
			}
			else if (RandomSide == 1)
			{
				SpawnRotation = FRotator(0, 45, 0);
				SpawnLocation = FVector(FMath::RandRange(DistanceFromWalls, FloorLength - (3 * DistanceFromWalls)),
										FMath::RandRange(DistanceFromWalls, FloorWidth - (3 * DistanceFromWalls)), ZHeight);
			}
			else if (RandomSide == 2)
			{
				SpawnRotation = FRotator(0, 90, 0);
				SpawnLocation = FVector(FMath::RandRange(DistanceFromWalls, FloorLength - DistanceFromWalls),
						FMath::RandRange(DistanceFromWalls, FloorWidth - (4 * DistanceFromWalls)), ZHeight);
			}
			else if (RandomSide == 3)
			{
				SpawnRotation = FRotator(0, 135, 0);
				SpawnLocation = FVector(FMath::RandRange((3 * DistanceFromWalls), FloorLength - DistanceFromWalls),
										FMath::RandRange(DistanceFromWalls, FloorWidth - (3 * DistanceFromWalls)), ZHeight);
			}
		}
		else  // every other object
		{
			SpawnLocation = FVector(FMath::RandRange(DistanceFromWalls, FloorLength - DistanceFromWalls), FMath::RandRange(DistanceFromWalls, FloorWidth - DistanceFromWalls), ZHeight);
			SpawnRotation = FRotator(0, FMath::RandRange(0.f, 360.f), 0);
		}
		
		FActorSpawnParameters SpawnInfo;
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ObjectClass, SpawnLocation, SpawnRotation, SpawnInfo);
		UE_LOG(LogTemp, Warning, TEXT("SpawnedActor: %s. Location: %s"), *SpawnedActor->GetName(), *SpawnedActor->GetActorLocation().ToString());
		
		if (bCheckCollision)
		{
			const UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(SpawnedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			FVector BoxHalfExtent = MeshComponent->GetStaticMesh()->GetBoundingBox().GetExtent();
			if (CheckOverlap(SpawnedActor, SpawnLocation, BoxHalfExtent))
			{
				UE_LOG(LogTemp, Warning, TEXT("BoxHalfExtent: %s"), *BoxHalfExtent.ToString());
				UE_LOG(LogTemp, Warning, TEXT("Overlap detected!"));
				SpawnedActor->Destroy();
				Attempts++;
				continue;
			}
		}

		SpawnedObjects++;
	}
}

/**
 * Check if the SpawnLocation overlaps with other objects given the BoxHalfExtent, the dimensions of the object's bounding box.
 * @param ActorToIgnore The actor to ignore when checking for overlap, usually the actor itself that is calling this function
 * @param SpawnLocation The location at which theobject is going to be spawned
 * @param BoxHalfExtent The half extent of the object's bounding box
 * @return TRUE Whether the object overlaps with other objects
 */
bool AProceduralFloor::CheckOverlap(const AActor* ActorToIgnore, const FVector& SpawnLocation, const FVector& BoxHalfExtent) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(ActorToIgnore); // Ignore self if needed

	TArray<FOverlapResult> HitResults;
	const bool bHit = GetWorld()->OverlapMultiByObjectType(
		HitResults,
		SpawnLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		FCollisionShape::MakeBox(BoxHalfExtent), // Adjust the shape based on your object's size
		CollisionParams
	);

	return bHit;
}
