#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ClipmapTerrainActor.generated.h"

struct FClipRotation
{
	float Angle;
	FVector Offset;
};

class UTerrainAsset;

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
	FClipRotation Rotations[4];

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
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY()
	bool bClipmapMeshDirty = true;
	bool bWindowTextureDirty = true;

	int WindowSize = 256;

public:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	int TileSize = 64;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	int ClipmapLevels = 8;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Transient)
	TObjectPtr<UTexture2D> WindowTexture;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	TObjectPtr<UTerrainAsset> TerrainAsset;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	TObjectPtr<UMaterialInterface> Material;
public:
	AClipmapTerrainActor();
protected:

	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& event) override;
	virtual void OnConstruction(const FTransform& transform) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; };
#endif

	virtual void Tick(float DeltaTime) override;

private:
	void CreateDynamicMaterial();
	void CreateWindowTexture();
	void CreateClipmapMesh();
	void UpdateClipmap();
	void UpdateParameters();
	FVector GetLocalCameraLocation() const;
};