#include "ClipmapDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IDesktopPlatform.h"
#include "IImageWrapperModule.h"
#include "DesktopPlatformModule.h"
#include "ClipmapTerrainActor.h"

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
        
        if (AClipmapTerrainActor* ClipmapActor = Cast<AClipmapTerrainActor>(ObjectsBeingCustomized[0].Get()))
        {
            IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("External File");

            Category.AddCustomRow(FText::FromString("Open File"))
                .ValueContent()
                [
                    SNew(SButton)
                        .Text(FText::FromString("Open File"))
                        .OnClicked_Lambda([&,ClipmapActor]()
                            {
                                IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
                                if (DesktopPlatform)
                                {
                                    const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

                                    FString FileTypes = TEXT("All Files (*.*)|*.*");
                                    TArray<FString> OutFiles;

                                    bool bOpened = DesktopPlatform->OpenFileDialog(
                                        ParentWindowHandle,
                                        TEXT("Choose File to Open"),
                                        TEXT(""),   // default path
                                        TEXT("png"),   // default file
                                        FileTypes,
                                        EFileDialogFlags::None,
                                        OutFiles
                                    );

                                    if (bOpened && OutFiles.Num() > 0)
                                    {
                                        ImportPNGHeightmap(ClipmapActor,OutFiles[0]);
                                    }
                                }
                                return FReply::Handled();
                            })
                ];
        }
    }
}

void FClipmapActorDetailsCustomization::ImportPNGHeightmap(AClipmapTerrainActor* ClipmapActor, const FString& path)
{
    static IImageWrapperModule* ImageWrapperModule = FModuleManager::LoadModulePtr<IImageWrapperModule>("ImageWrapper");
}