// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClipmapCoreEditor.h"
#include "ClipmapDetailCustomization.h"

IMPLEMENT_MODULE(FClipmapCoreEditor, ClipmapCoreEditor)

void FClipmapCoreEditor::StartupModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(
        "ClipmapTerrainActor",
        FOnGetDetailCustomizationInstance::CreateStatic(&FClipmapActorDetailsCustomization::MakeInstance)
    );
}

void FClipmapCoreEditor::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomClassLayout("ClipmapTerrainActor");
    }
}
