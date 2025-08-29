#include "ClipmapTerrainActor.h"
#include "ClipmapBuilder.h"
#if WITH_EDITOR
#include "Editor.h"
#include "LevelEditorViewport.h"
#endif
AClipmapTerrainActor::AClipmapTerrainActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CrossMeshInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cross Mesh Instance"), true);
	TileMeshInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Tile Mesh Instance"), true);
	FillerMeshInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Filler Mesh Instance"), true);
	TrimMeshInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Trim Mesh Instance"), true);
	SeamMeshInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Seam Mesh Instance"), true);

	CrossMeshInstance->bDisableCollision = true;
	CrossMeshInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CrossMeshInstance->SetCanEverAffectNavigation(false);
	CrossMeshInstance->SetMobility(EComponentMobility::Movable);
	CrossMeshInstance->SetGenerateOverlapEvents(false);
	CrossMeshInstance->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;

	TileMeshInstance->bDisableCollision = true;
	TileMeshInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TileMeshInstance->SetCanEverAffectNavigation(false);
	TileMeshInstance->SetMobility(EComponentMobility::Movable);
	TileMeshInstance->SetGenerateOverlapEvents(false);
	TileMeshInstance->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;

	FillerMeshInstance->bDisableCollision = true;
	FillerMeshInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FillerMeshInstance->SetCanEverAffectNavigation(false);
	FillerMeshInstance->SetMobility(EComponentMobility::Movable);
	FillerMeshInstance->SetGenerateOverlapEvents(false);
	FillerMeshInstance->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;

	TrimMeshInstance->bDisableCollision = true;
	TrimMeshInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TrimMeshInstance->SetCanEverAffectNavigation(false);
	TrimMeshInstance->SetMobility(EComponentMobility::Movable);
	TrimMeshInstance->SetGenerateOverlapEvents(false);
	TrimMeshInstance->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;

	SeamMeshInstance->bDisableCollision = true;
	SeamMeshInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SeamMeshInstance->SetCanEverAffectNavigation(false);
	SeamMeshInstance->SetReverseCulling(true);
	SeamMeshInstance->SetMobility(EComponentMobility::Movable);
	SeamMeshInstance->SetGenerateOverlapEvents(false);
	SeamMeshInstance->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Static;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Terrain Root");

	SeamMeshInstance->SetupAttachment(RootComponent);
	TrimMeshInstance->SetupAttachment(RootComponent);
	FillerMeshInstance->SetupAttachment(RootComponent);
	TileMeshInstance->SetupAttachment(RootComponent);
	CrossMeshInstance->SetupAttachment(RootComponent);
}

void AClipmapTerrainActor::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
void AClipmapTerrainActor::OnConstruction(const FTransform& transform)
{
	Super::OnConstruction(transform);
	if (bClipmapMeshDirty)
	{
		bClipmapMeshDirty = false;
		GenerateClipmapMesh();
	}
}
#endif

//Clears the SMIs and recreates the meshes needed
void AClipmapTerrainActor::GenerateClipmapMesh()
{
	Rotations.SetNum(4);

	TileMap.Reset();
	Fillers.Reset();
	Trims.Reset();
	Seams.Reset();
	const int32 NUM_CLIPMAP_LEVELS = ClipmapLevels;
	TileMap.SetNum(NUM_CLIPMAP_LEVELS * 16);
	Fillers.SetNum(NUM_CLIPMAP_LEVELS);

	Trims.SetNum(NUM_CLIPMAP_LEVELS);

	Seams.SetNum(NUM_CLIPMAP_LEVELS);

	CrossMeshInstance->ClearInstances();
	TileMeshInstance->ClearInstances();
	FillerMeshInstance->ClearInstances();
	TrimMeshInstance->ClearInstances();
	SeamMeshInstance->ClearInstances();
	CrossMeshInstance->SetStaticMesh(ClipmapBuilder::CrossMesh(TileSize));
	TileMeshInstance->SetStaticMesh(ClipmapBuilder::TileMesh(TileSize));
	FillerMeshInstance->SetStaticMesh(ClipmapBuilder::FillerMesh(TileSize));
	TrimMeshInstance->SetStaticMesh(ClipmapBuilder::TrimMesh(TileSize));
	SeamMeshInstance->SetStaticMesh(ClipmapBuilder::SeamMesh(TileSize));

	CrossInstanceID = FPrimitiveInstanceId();
}

void AClipmapTerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateClipmap();
}
void AClipmapTerrainActor::UpdateClipmap()
{
	const int32 EXTRA_SCALE = 100;
	const int TILE_RESOLUTION = TileSize;
	const int CLIPMAP_RESOLUTION = TILE_RESOLUTION * 4 + 1;
	const int CLIPMAP_VERT_RESOLUTION = CLIPMAP_RESOLUTION + 1;
	const int NUM_CLIPMAP_LEVELS = ClipmapLevels;
	const FVector ViewPosition = GetLocalCameraLocation();

	FVector snappedPos = FVector(FMath::Floor(ViewPosition.X / EXTRA_SCALE), FMath::Floor(ViewPosition.Y / EXTRA_SCALE), 0);



	if (!CrossInstanceID.IsValid())
	{
		CrossInstanceID = CrossMeshInstance->AddInstanceById(FTransform(FRotator::ZeroRotator, FVector(snappedPos) * EXTRA_SCALE, FVector(EXTRA_SCALE, EXTRA_SCALE, 1)), true);
	}
	else
	{
		CrossMeshInstance->UpdateInstanceTransformById(CrossInstanceID, FTransform(FRotator::ZeroRotator, FVector(snappedPos) * EXTRA_SCALE, FVector(EXTRA_SCALE, EXTRA_SCALE, 1)), true);
	}
	float maxX = 0, maxY = 0, minX = 0, minY = 0;
	for (int32 i = 0; i < NUM_CLIPMAP_LEVELS * 16; i++)
	{
		const int32 level = i / 16;
		const int32 xy = i - level * 16;
		const int y = xy / 4;
		const int x = xy - y * 4;


		if (level != 0 && (x == 1 || x == 2) && (y == 1 || y == 2))
			continue;

		const bool firstLevel = level == 0;

		const int32 scale = EXTRA_SCALE << level;
		snappedPos = FVector(FMath::Floor(ViewPosition.X / scale), FMath::Floor(ViewPosition.Y / scale), 0.0f) * scale;
		const FVector tileSize = FVector(TILE_RESOLUTION * EXTRA_SCALE << level, TILE_RESOLUTION * EXTRA_SCALE << level, 0);

		const FVector base = FVector(snappedPos.X, snappedPos.Y, 0) - tileSize * 2;
		const FVector fill = FVector(x >= 2 ? 1 : 0, y >= 2 ? 1 : 0, 0) * scale;
		const FVector tile_bl = base + FVector(x, y, 0) * tileSize + fill;
		if (tile_bl.X > maxX)
		{
			maxX = tile_bl.X;
		}
		if (tile_bl.Y > maxY)
		{
			maxY = tile_bl.Y;
		}
		if (tile_bl.X < minX)
		{
			minX = tile_bl.X;
		}
		if (tile_bl.Y < minY)
		{
			minY = tile_bl.Y;
		}
		FPrimitiveInstanceId& idEntry = TileMap[i];
		if (idEntry.IsValid())
		{
			TileMeshInstance->UpdateInstanceTransformById(idEntry, FTransform(FRotator::ZeroRotator, tile_bl, FVector(scale, scale, 1.0f)), true);
		}
		else
		{
			idEntry = TileMeshInstance->AddInstanceById(FTransform(FRotator::ZeroRotator, tile_bl, FVector(scale, scale, 1.0f)), true);
			TileMeshInstance->SetCustomDataValueById(idEntry, 0, level);
		}

	}
	for (int32 level = 0; level < NUM_CLIPMAP_LEVELS; level++)
	{
		const int32 scale = EXTRA_SCALE << level;
		snappedPos = FVector(FMath::Floor(ViewPosition.X / scale), FMath::Floor(ViewPosition.Y / scale), 0.0f) * scale;
		//const FVector tileSize = FVector(TILE_RESOLUTION * EXTRA_SCALE << level, TILE_RESOLUTION * EXTRA_SCALE << level, 0);


		bool firstLevel = level == 0;






		FPrimitiveInstanceId& fillerIdEntry = Fillers[level];
		if (fillerIdEntry.IsValid())
		{
			FillerMeshInstance->UpdateInstanceTransformById(fillerIdEntry, FTransform(FRotator::ZeroRotator, snappedPos, FVector(scale, scale, 1.0f)), true, false);
		}
		else
		{
			fillerIdEntry = FillerMeshInstance->AddInstanceById(FTransform(FRotator::ZeroRotator, snappedPos, FVector(scale, scale, 1.0f)), true);

		}

		if (level != NUM_CLIPMAP_LEVELS - 1)
		{
			const int next_scale = scale * 2;
			const FVector s = ((ViewPosition) / next_scale);

			const FVector next_snapped_position = FVector(FMath::Floor(s.X), FMath::Floor(s.Y), 0.0f) * next_scale;

			const FVector tile_centre = snappedPos + FVector(scale * 0.5, scale * 0.5, 0.0f);

			const FVector d = (ViewPosition)-next_snapped_position;
			int r = 0;
			r |= d.X >= scale ? 0 : 2;
			r |= d.Y >= scale ? 0 : 1;
			const int offset = CLIPMAP_VERT_RESOLUTION * scale;

			Rotations[0].Angle = 0.0f;
			Rotations[0].Offset.X = 0.0f;
			Rotations[0].Offset.Y = 0.0f;

			Rotations[1].Angle = 90.0f;
			Rotations[1].Offset.X = offset;
			Rotations[1].Offset.Y = 0.0f;

			Rotations[2].Angle = 270.0f;
			Rotations[2].Offset.X = 0.0f;
			Rotations[2].Offset.Y = offset;

			Rotations[3].Angle = 180.0f;
			Rotations[3].Offset.X = offset;
			Rotations[3].Offset.Y = offset;

			const FVector next_base = next_snapped_position - FVector(TILE_RESOLUTION * EXTRA_SCALE << (level + 1), TILE_RESOLUTION * EXTRA_SCALE << (level + 1), 0);
			const FVector trimPos = FVector(next_base.X + Rotations[r].Offset.Y, next_base.Y + Rotations[r].Offset.X, 0);

			FPrimitiveInstanceId& trimIdEntry = Trims[level];
			if (trimIdEntry.IsValid())
			{
				TrimMeshInstance->UpdateInstanceTransformById(trimIdEntry, FTransform(FRotator(0.0f, -Rotations[r].Angle, 0.0f), trimPos, FVector(scale, scale, 1.0f)), true, false);
			}
			else
			{
				trimIdEntry = TrimMeshInstance->AddInstanceById(FTransform(FRotator(0.0f, -Rotations[r].Angle, 0.0f), trimPos, FVector(scale, scale, 1.0f)), true);
			}


			FPrimitiveInstanceId& seamIdEntry = Seams[level];
			if (seamIdEntry.IsValid())
			{
				SeamMeshInstance->UpdateInstanceTransformById(seamIdEntry, FTransform(FRotator::ZeroRotator, next_base, FVector(scale, scale, 1.0f)), true, false);
			}
			else
			{
				seamIdEntry = SeamMeshInstance->AddInstanceById(FTransform(FRotator::ZeroRotator, next_base, FVector(scale, scale, 1.0f)), true);
			}

		}
	}
}
FVector AClipmapTerrainActor::GetLocalCameraLocation() const
{
	
#if WITH_EDITOR
	UWorld* world = GetWorld();
	if (!world)
	{
		return FVector::ZeroVector;
	}
	if (world->IsEditorWorld() && GCurrentLevelEditingViewportClient)
	{
		FViewportCameraTransform& ViewTransform = GCurrentLevelEditingViewportClient->GetViewTransform();
		return ViewTransform.GetLocation();
	}
#endif
	return GetActorLocation();
}