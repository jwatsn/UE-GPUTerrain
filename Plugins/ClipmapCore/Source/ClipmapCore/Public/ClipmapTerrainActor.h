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
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	bool bClipmapMeshDirty = true;
	bool bWindowTextureDirty = true;
	bool bFirstUpdate = false;
	FVector LastViewPosition;
	int WindowSize = 512;



public:

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> CrossMeshInstance;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> TileMeshInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInstancedStaticMeshComponent> FillerMeshInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInstancedStaticMeshComponent> TrimMeshInstance;
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> SeamMeshInstance;
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Clipmap Settings")
	int TileSize = 63;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Clipmap Settings")
	int ClipmapLevels = 8;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Clipmap Settings")
	int ClipmapScale = 100;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Transient)
	TObjectPtr<UTexture2D> WindowTexture;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Terrain Settings")
	TObjectPtr<UTerrainAsset> TerrainAsset;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Terrain Settings")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Terrain Settings")
	FVector TerrainScale = FVector(100, 100, 100);
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
	void UpdateWindowTexture();
	FVector GetLocalCameraLocation() const;
};