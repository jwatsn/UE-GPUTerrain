#pragma once

#include "CoreMinimal.h"
#include "Chaos/HeightField.h"
#include "TerrainAsset.generated.h"

namespace Chaos
{
	class FHeightField;
}

struct FHeightmapOldSettings
{
	bool SRGB;
	EPixelFormat PixelFormat;
	TextureCompressionSettings CompressionSettings;
	TextureMipGenSettings MipSettings;
};

UCLASS(Blueprintable)
class CLIPMAPCORE_API UTerrainAsset : public UDataAsset
{
	GENERATED_BODY()

	

public:

	TArray64<uint16> Heights;


protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChanged) override;
#endif

	virtual void Serialize(FArchive& Ar) override;
};