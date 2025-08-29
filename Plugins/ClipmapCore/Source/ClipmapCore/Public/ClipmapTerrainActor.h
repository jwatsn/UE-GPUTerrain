#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ClipmapTerrainActor.generated.h"

struct FClipRotation
{
	float Angle;
	FVector Offset;
};

UCLASS()
class CLIPMAPCORE_API AClipmapTerrainActor : public AActor
{
	GENERATED_BODY()

	//Static mesh instance ID's
	FPrimitiveInstanceId CrossInstanceID;
	TArray<FPrimitiveInstanceId> TileMap;
	TArray<FPrimitiveInstanceId> Fillers;
	TArray<FPrimitiveInstanceId> Trims;
	TArray<FPrimitiveInstanceId> Seams;

	//Reusable rotation data for static mesh instances
	TArray<FClipRotation> Rotations;

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> CrossMeshInstance;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> TileMeshInstance;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> FillerMeshInstance;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> TrimMeshInstance;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> SeamMeshInstance;

	UPROPERTY()
	bool bClipmapMeshDirty = true;

public:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	int TileSize = 64;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	int ClipmapLevels = 8;
public:
	AClipmapTerrainActor();
protected:

	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& transform) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; };
#endif

	virtual void Tick(float DeltaTime) override;

private:

	void GenerateClipmapMesh();
	void UpdateClipmap();
	FVector GetLocalCameraLocation() const;
};