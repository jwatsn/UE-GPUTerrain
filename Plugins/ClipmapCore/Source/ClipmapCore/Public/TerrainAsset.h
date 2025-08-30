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

	int Width = 0;
	int Height = 0;

public:

	TArray64<uint16> Heights;

public:
	bool UpdateWindowTexture(int x, int y, UTexture2D* windowTexture);
	int32 GetWidth() const { return Width; };
	int32 GetHeight() const { return Height; };
	void SetSize(int32 width, int32 height)
	{
		Width = width;
		Height = height;
	}

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChanged) override;
#endif

	virtual void Serialize(FArchive& Ar) override;
};