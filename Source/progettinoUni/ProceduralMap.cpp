// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralMap.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProceduralMap::AProceduralMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	FloorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComponent"));
	FloorMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProceduralMap::BeginPlay()
{
	Super::BeginPlay();

	ProgrammaticallyPlaceFloor();

	PlaceWalls();
	
	ProcedurallySpawnObjects(BP_Archway, 0);
	ProcedurallySpawnObjects(BP_Pillar, 0, true);
	ProcedurallySpawnObjects(BP_MovableBlocks, 0, true, 55);
	ProcedurallySpawnObjects(BP_Coin, 7, true, 50);
	ProcedurallySpawnObjects(BP_Gem, 3, true, 100);
}

// Called every frame
void AProceduralMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralMap::PlaceWalls()
{
	// place the walls on the edges of the floor
	constexpr int32 WallThickness = 100;  // thickness of the edge's walls is 100cm or 0.1m in scale language
	constexpr int32 WallHeight = 300;  // 300cm or 3m in scale language
	
	// left wall
	FVector LeftWallLocation = FVector(0, -((FloorWidth / 2) + (WallThickness / 2)), WallHeight / 2);  // we need to move of half the thickness of a wall to the left to not occupy space on the floor, and let the wall stay on the exact edge
	FRotator LeftWallRotation = FRotator(0, 0, 0);
	FVector LeftWallScale = FVector(FloorWidth / 100, 1.0f, WallHeight / 100); // we need to divide by 100 because the positins of the world are in cm and we need to convert it to m for the scale
	FTransform LeftWallTransform(LeftWallRotation, LeftWallLocation, LeftWallScale);
	AActor* LeftWall = GetWorld()->SpawnActor<AActor>(BP_Wall, LeftWallTransform);
	LeftWall->SetActorScale3D(LeftWallScale);
	
	// right wall
	FVector RightWallLocation = FVector(0, (FloorWidth / 2) + (WallThickness / 2), WallHeight / 2);
	FRotator RightWallRotation = FRotator(0, 0, 0);
	FVector RightWallScale = FVector(FloorWidth / 100, 1.0f,  WallHeight / 100);
	FTransform RightWallTransform(RightWallRotation, RightWallLocation, RightWallScale);
	AActor* RightWall = GetWorld()->SpawnActor<AActor>(BP_Wall, RightWallTransform);
	RightWall->SetActorScale3D(RightWallScale);
	
	// top wall
	FVector TopWallLocation = FVector((FloorLength / 2) + (WallThickness / 2), 0, WallHeight / 2);
	FRotator TopWallRotation = FRotator(0, 0, 0);
	FVector TopWallScale = FVector(1.0f, (FloorWidth / 100) + (WallThickness * 2 / 100), WallHeight / 100);  // we need to add the thickness of two edge's walls to the width of the top and bottom walls to close the gap between the left and right walls
	FTransform TopWallTransform(TopWallRotation, TopWallLocation, TopWallScale);
	AActor* TopWall = GetWorld()->SpawnActor<AActor>(BP_Wall, TopWallTransform);
	TopWall->SetActorScale3D(TopWallScale);
	
	// bottom wall
	FVector BottomWallLocation = FVector(-((FloorLength / 2) + (WallThickness / 2)), 0, WallHeight / 2);
	FRotator BottomWallRotation = FRotator(0, 0, 0);
	FVector BottomWallScale = FVector(1.0f, (FloorWidth / 100) + (WallThickness * 2 / 100), WallHeight / 100);
	FTransform BottomWallTransform(BottomWallRotation, BottomWallLocation, BottomWallScale);
	AActor* BottomWall = GetWorld()->SpawnActor<AActor>(BP_Wall, BottomWallTransform);
	BottomWall->SetActorScale3D(BottomWallScale);
	
}

void AProceduralMap::ProgrammaticallyPlaceFloor()
{
	AProceduralMap* ProceduralMapBPActor = Cast<AProceduralMap>(UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralMap::StaticClass()));
	if (ProceduralMapBPActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("BP_Floor is null!"));
		return;
	}
	// scale the actor floor
	ProceduralMapBPActor->SetActorScale3D(FVector(10.f, 10.f, 1.f));  // remember these values are in m

	// Get the dimensions of the floor
	FloorLength = ProceduralMapBPActor->GetActorScale().X * 100.0;
	UE_LOG(LogTemp, Warning, TEXT("FloorLength: %d"), FloorLength);
	FloorWidth = ProceduralMapBPActor->GetActorScale().Y * 100.0;
	UE_LOG(LogTemp, Warning, TEXT("FloorWidth: %d"), FloorWidth);
}

/**
 * @brief Procedurally spawn objects in the floor, takes into account to not spawn objects inside the walls on the edge of the floor,
 * it tries to spawn the objects in a random location and rotation, if bCheckCollision is true it checks if the object overlaps with other objects,
 * if it does it destroys the object and tries again, if it fails to spawn the object after 100 attempts it stops trying.
 * If the object is an archway it rotate the objects only in 4 directions (0, 45, 90, 135) and place them farther from the edges of the floor avoiding clipping with the edge's walls.
 * @param ObjectClass The class of the object to spawn
 * @param NumberOfObjects The number of objects to spawn, defaults to 1
 * @param bCheckCollision Whether to check for collision with other objects, defaults to false
 * @param ZHeight The height at which to spawn the objects, defaults to 0
 */
void AProceduralMap::ProcedurallySpawnObjects(UClass* ObjectClass, const int32 NumberOfObjects, const bool bCheckCollision, const float ZHeight) const
{
	const float HalfFloorLength = FloorLength / 2; // to shorten the calculations
	const float HalfFloorWidth = FloorWidth / 2;
	constexpr int32 DistanceFromWalls = 100;  // min spawning distance from the walls on the edge of the floor in cm
	
	if (HalfFloorLength <= DistanceFromWalls || HalfFloorWidth <= DistanceFromWalls)
	{
		UE_LOG(LogTemp, Error, TEXT("The floor is too small to spawn objects!"));
		return;
	}
	
	int32 SpawnedObjects = 0;
	constexpr int32 MaxAttempts = 100;
	int32 Attempts = 0;
	while (SpawnedObjects < NumberOfObjects && Attempts < MaxAttempts)
	{
		FVector SpawnLocation;
		FRotator SpawnRotation;
		if (ObjectClass == BP_Archway)
		{
			int32 RandomSide = FMath::RandRange(0, 3);  // only 4 rotations
			if (RandomSide == 0) /* | */
			{
				SpawnRotation = FRotator(0, 0, 0);
				SpawnLocation = FVector(FMath::RandRange(-HalfFloorLength + (4 * DistanceFromWalls), HalfFloorLength - (4 * DistanceFromWalls)),
										FMath::RandRange(-HalfFloorWidth + DistanceFromWalls, HalfFloorWidth - DistanceFromWalls), ZHeight);
			}
			else if (RandomSide == 1) /* / */
			{
				SpawnRotation = FRotator(0, 45, 0);
				SpawnLocation = FVector(FMath::RandRange( -HalfFloorLength + 3 * DistanceFromWalls, HalfFloorLength - (3 * DistanceFromWalls)),
										FMath::RandRange(-HalfFloorWidth + 3 * DistanceFromWalls, HalfFloorWidth - (3 * DistanceFromWalls)), ZHeight);
			}
			else if (RandomSide == 2) /* - */
			{
				SpawnRotation = FRotator(0, 90, 0);
				SpawnLocation = FVector(FMath::RandRange(-HalfFloorLength + DistanceFromWalls, HalfFloorLength - DistanceFromWalls),
										FMath::RandRange(-HalfFloorLength + 4 * DistanceFromWalls, HalfFloorWidth - (4 * DistanceFromWalls)), ZHeight);
			}
			else if (RandomSide == 3) /* \ */
			{
				SpawnRotation = FRotator(0, 135, 0);
				SpawnLocation = FVector(FMath::RandRange(-HalfFloorLength + (3 * DistanceFromWalls), HalfFloorLength - 3 * DistanceFromWalls),
										FMath::RandRange(-HalfFloorWidth + 3 * DistanceFromWalls, HalfFloorWidth - (3 * DistanceFromWalls)), ZHeight);
			}
		}
		else  // every other object
		{
			SpawnLocation = FVector(FMath::RandRange(-HalfFloorLength + DistanceFromWalls, HalfFloorLength - DistanceFromWalls),
									FMath::RandRange(-HalfFloorWidth + DistanceFromWalls, HalfFloorWidth - DistanceFromWalls), ZHeight);
			SpawnRotation = FRotator(0, FMath::RandRange(0.f, 360.f), 0);
		}
		
		FActorSpawnParameters SpawnInfo;
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ObjectClass, SpawnLocation, SpawnRotation, SpawnInfo);
		UE_LOG(LogTemp, Warning, TEXT("SpawnedActor: %s. Location: %s"), *SpawnedActor->GetName(), *SpawnedActor->GetActorLocation().ToString());
		
		if (bCheckCollision)
		{
			const UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(SpawnedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			FVector BoxHalfExtent = MeshComponent->GetStaticMesh()->GetBoundingBox().GetExtent();
			if (IsActorColliding(SpawnedActor, SpawnLocation, BoxHalfExtent))
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
 * Check if the ActorSpawnLocation overlaps with other objects given the ActorBoxHalfExtent, the dimensions of the object's bounding box.
 * @param ActorToCheck The actor to ignore when checking for overlap, usually the actor itself that is calling this function
 * @param ActorSpawnLocation The location at which theobject is going to be spawned
 * @param ActorBoxHalfExtent The half extent of the object's bounding box
 * @return TRUE Whether the object overlaps with other objects
 */
bool AProceduralMap::IsActorColliding(const AActor* ActorToCheck, const FVector& ActorSpawnLocation, const FVector& ActorBoxHalfExtent) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(ActorToCheck); // Ignore self if needed

	TArray<FOverlapResult> HitResults;
	const bool bHit = GetWorld()->OverlapMultiByObjectType(
		HitResults,
		ActorSpawnLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		FCollisionShape::MakeBox(ActorBoxHalfExtent), // Adjust the shape based on your object's size
		CollisionParams
	);

	return bHit;
}
