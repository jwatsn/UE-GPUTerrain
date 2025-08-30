#include "TerrainAsset.h"
#include "Rendering/Texture2DResource.h"
#if WITH_EDITOR
void UTerrainAsset::PostEditChangeProperty(FPropertyChangedEvent& propertyChanged)
{
	Super::PostEditChangeProperty(propertyChanged);

}
#endif
void UTerrainAsset::UpdateWindowTexture(int x, int y, UTexture2D* windowTexture)
{
	if (!windowTexture || !windowTexture->GetPlatformData() || windowTexture->GetPlatformData()->Mips.IsEmpty())
	{
		return;
	}
	const int windowWidth = windowTexture->GetSizeX();
	const int windowHeight = windowTexture->GetSizeY();

	const int X = x - windowWidth / 2.0;
	const int Y = y - windowHeight / 2.0;

	FTexture2DResource* TextureResource = nullptr;
	if (FTextureResource* resource = windowTexture->GetResource())
	{
		TextureResource = static_cast<FTexture2DResource*>(resource);
	}
	if (!TextureResource)
	{
		return;
	}
	TArray<FUpdateTextureRegion2D> updateRegions;
	int wrappedX = (wrappedX = X % Width) >= 0 ? wrappedX : Width + wrappedX;
	int wrappedY = (wrappedY = Y % Height) >= 0 ? wrappedY : Height + wrappedY;

	int offsetY = 0;
	
	while (offsetY < windowHeight)
	{
		const int sizeYLeft = windowHeight - offsetY;
		const int sizeY = Height - wrappedY;
		const int sizeYToAdd = FMath::Min(sizeYLeft, sizeY);
		int offsetX = 0;
		while (offsetX < windowWidth)
		{

			const int sizeXLeft = windowWidth - offsetX;
			const int sizeX = Width - wrappedX;
			const int sizeXToAdd = FMath::Min(sizeXLeft, sizeX);

			const uint32 SrcStride = Width * sizeof(uint16);
			//uint16* buffer = Heights.GetData() + (wrappedY * Width + wrappedX);
			updateRegions.Add(FUpdateTextureRegion2D(offsetX, offsetY, wrappedX, wrappedY, sizeXToAdd, sizeYToAdd));
			
			offsetX += sizeXToAdd;
			wrappedX = (wrappedX + sizeXToAdd) % Width;

		}
		offsetY += sizeYToAdd;
		wrappedY = (wrappedY + sizeYToAdd) % Height;

	}
	if(TextureResource->GetTexture2DRHI())
	{
		ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)
			([TextureResource, Regions = MoveTemp(updateRegions),&heightData=Heights,stride=Width * sizeof(uint16)](FRHICommandList& RHICmdList)
				{
					if (FRHITexture* textureRHI = TextureResource->GetTexture2DRHI())
					{
						for (const FUpdateTextureRegion2D& Region : Regions)
						{
							RHIUpdateTexture2D(
								textureRHI,
								0, // Mip level
								Region,
								stride,
								reinterpret_cast<uint8*>(heightData.GetData())
							);

						}
					}
						
						
					
				});
	}
}
void UTerrainAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	int32 width = Width;
	int32 height = Height;
	Ar << width;
	Ar << height;
	Width = width;
	Height = height;
	int64 numHeights = width * height;
	Heights.SetNum(numHeights);
	for (int64 i = 0; i < numHeights; i++)
	{
		Ar << Heights[i];
	}
}