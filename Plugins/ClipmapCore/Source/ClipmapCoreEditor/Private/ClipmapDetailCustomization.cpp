#include "ClipmapDetailCustomization.h"
#include "ClipmapCoreLogging.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IDesktopPlatform.h"
#include "IImageWrapperModule.h"
#include "DesktopPlatformModule.h"
#include "TerrainAsset.h"

TSharedRef<IDetailCustomization> FClipmapActorDetailsCustomization::MakeInstance()
{
    return MakeShareable(new FClipmapActorDetailsCustomization);
}

void FClipmapActorDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    // Get the objects being customized
    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

    if (ObjectsBeingCustomized.Num() == 1)
    {
        
        if (UTerrainAsset* Asset = Cast<UTerrainAsset>(ObjectsBeingCustomized[0].Get()))
        {
            IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("External File");

            Category.AddCustomRow(FText::FromString("Open File"))
                .ValueContent()
                [
                    SNew(SButton)
                        .Text(FText::FromString("Open File"))
                        .OnClicked_Lambda([&, Asset]()
                            {
                                IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
                                if (DesktopPlatform)
                                {
                                    const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

                                    FString FileTypes = TEXT("Heightmap Files (*.png)|*.png");
                                    TArray<FString> OutFiles;

                                    bool bOpened = DesktopPlatform->OpenFileDialog(
                                        ParentWindowHandle,
                                        TEXT("Choose File to Open"),
                                        TEXT(""),   // default path
                                        TEXT(""),   // default file
                                        FileTypes,
                                        EFileDialogFlags::None,
                                        OutFiles
                                    );

                                    if (bOpened && OutFiles.Num() > 0)
                                    {
                                        ImportPNGHeightmap(Asset,OutFiles[0]);
                                    }
                                }
                                return FReply::Handled();
                            })
                ];
        }
    }
}

void FClipmapActorDetailsCustomization::ImportPNGHeightmap(UTerrainAsset* Asset, const FString& path)
{
    static IImageWrapperModule* ImageWrapperModule = FModuleManager::LoadModulePtr<IImageWrapperModule>("ImageWrapper");

    if (ImageWrapperModule == nullptr)
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap: Could not load ImageWrapper module"));
        return;
    }
    if (!FPaths::FileExists(path))
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap: file does not exist"));
        return;
    }
    TArray<uint8> rawImageData;
    if (!FFileHelper::LoadFileToArray(rawImageData, *path))
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap: could not read file"));
        return;
    }
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);
    if (!ImageWrapper.IsValid())
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap"));
        return;
    }
    if (!ImageWrapper->SetCompressed(rawImageData.GetData(), rawImageData.Num()))
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap: Invalid image file"));
        return;
    }
    const int bitDepth = ImageWrapper->GetBitDepth();
    const ERGBFormat format = ImageWrapper->GetFormat();
    if (format == ERGBFormat::Invalid)
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap: Invalid image format"));
        return;
    }
    UE_LOG(LogClipmapCore, Log, TEXT("Format: %d BitDepth: %d"), (int)format, bitDepth);
    
    TArray<uint8> cookData;
    if (!ImageWrapper->GetRaw(format, bitDepth, cookData))
    {
        UE_LOG(LogClipmapCore, Error, TEXT("Error importing heightmap: error reading image"));
        return;
    }
    FString pathPart, filenamePart, extensionPart;
    FPaths::Split(path, pathPart, filenamePart, extensionPart);

    const int32 width = ImageWrapper->GetWidth();
    const int32 height = ImageWrapper->GetHeight();

    TArray64<uint16>& Heights = Asset->Heights;
    Heights.SetNum(width * height);
    switch (format)
    {
    case ERGBFormat::Gray:
        if (bitDepth == 8)
        {
            for (int i = 0; i < cookData.Num(); i++)
            {
                Heights[i] = cookData[i] / 255.0 * UINT16_MAX;
            }
        }
        else if (bitDepth == 16)
        {
            const uint16* rawImageData16 = (uint16*)cookData.GetData();
            uint16* heightData = Heights.GetData();
            FMemory::Memcpy(heightData, rawImageData16, cookData.Num());
        }
    default:
        if (bitDepth == 8)
        {
            const FColor* colorData = (FColor*)cookData.GetData();
            for (int i = 0; i < width * height; i++)
            {
                Heights[i] = colorData[i].R / 255.0 * UINT16_MAX;
            }
        }
        else if (bitDepth == 16)
        {
            const FLinearColor* colorData = (FLinearColor*)cookData.GetData();
            for (int i = 0; i < width * height; i++)
            {
                Heights[i] = colorData[i].R * UINT16_MAX;
            }
        }
    }


    /*UTexture2D* newTexture = NewObject<UTexture2D>(Asset, *filenamePart);
    Asset->HeightmapTexture = newTexture;

    newTexture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
    newTexture->SRGB = false;
    newTexture->MipGenSettings = TMGS_NoMipmaps;

    newTexture->SetPlatformData(new FTexturePlatformData());
    newTexture->GetPlatformData()->SizeX = ImageWrapper->GetWidth();
    newTexture->GetPlatformData()->SizeY = ImageWrapper->GetHeight();
    newTexture->GetPlatformData()->SetNumSlices(1);
    switch (bitDepth)
    {
    case 8:
        newTexture->GetPlatformData()->PixelFormat = EPixelFormat::PF_G8;
        break;
    case 16:
        newTexture->GetPlatformData()->PixelFormat = EPixelFormat::PF_G16;
        break;
    }*/
    
}