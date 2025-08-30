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

	FTexture2DResource* TextureResource = nullptr;
	if (FTextureResource* resource = windowTexture->GetResource())
	{
		TextureResource = static_cast<FTexture2DResource*>(resource);
	}
	if (!TextureResource)
	{
		return;
	}
	if(TextureResource->GetTexture2DRHI())
	{
		ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)
			([&, TextureResource, WindowWidth = windowTexture->GetSizeX(), WindowHeight = windowTexture->GetSizeY(),X=x,Y=y](FRHICommandList& RHICmdList)
				{
					int wrappedX = (wrappedX = X % Width) >= 0 ? wrappedX : Width + wrappedX;
					int wrappedY = (wrappedY = Y % Height) >= 0 ? wrappedY : Height + wrappedY;
					
					int offsetY = 0;
					FRHITexture* textureRHI = TextureResource->GetTexture2DRHI();
					while (offsetY < WindowHeight)
					{
						const int sizeYLeft = WindowHeight - offsetY;
						const int sizeY = Height - wrappedY;
						const int sizeYToAdd = FMath::Min(sizeYLeft, sizeY);
						int offsetX = 0;
						while (offsetX < WindowWidth)
						{
							
							const int sizeXLeft = WindowWidth - offsetX;
							const int sizeX = Width - wrappedX;
							const int sizeXToAdd = FMath::Min(sizeXLeft, sizeX);
							
							const uint32 SrcStride = Width * sizeof(uint16);
							//uint16* buffer = Heights.GetData() + (wrappedY * Width + wrappedX);
							const FUpdateTextureRegion2D Region(offsetX, offsetY, wrappedX, wrappedY, sizeXToAdd, sizeYToAdd);
							RHIUpdateTexture2D(
								textureRHI,
								0, // Mip level
								Region,
								SrcStride,
								reinterpret_cast<uint8*>(Heights.GetData())
							);
							UE_LOG(LogTemp, Log, TEXT("offsetX: %d offsetY: %d xToAdd: %d yToAdd: %d wrappedX: %d wrappedY: %d"), offsetX, offsetY, sizeXToAdd, sizeYToAdd, wrappedX, wrappedY);
							offsetX += sizeXToAdd;
							wrappedX = (wrappedX + sizeXToAdd) % Width;
							
						}
						offsetY += sizeYToAdd;
						wrappedY = (wrappedY + sizeYToAdd) % Height;
						
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