// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEGPUClipmaps : ModuleRules
{
	public UEGPUClipmaps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UEGPUClipmaps",
			"UEGPUClipmaps/Variant_Platforming",
			"UEGPUClipmaps/Variant_Platforming/Animation",
			"UEGPUClipmaps/Variant_Combat",
			"UEGPUClipmaps/Variant_Combat/AI",
			"UEGPUClipmaps/Variant_Combat/Animation",
			"UEGPUClipmaps/Variant_Combat/Gameplay",
			"UEGPUClipmaps/Variant_Combat/Interfaces",
			"UEGPUClipmaps/Variant_Combat/UI",
			"UEGPUClipmaps/Variant_SideScrolling",
			"UEGPUClipmaps/Variant_SideScrolling/AI",
			"UEGPUClipmaps/Variant_SideScrolling/Gameplay",
			"UEGPUClipmaps/Variant_SideScrolling/Interfaces",
			"UEGPUClipmaps/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
