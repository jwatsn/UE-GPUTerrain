// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ClipmapCoreEditor : ModuleRules
{
	public ClipmapCoreEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"ClipmapCore"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
				// ... add private dependencies that you statically link with here ...	
			}
			);

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

			PrivateDependencyModuleNames.AddRange(new string[] {"UnrealEd", "UMGEditor","DetailCustomizations", "PropertyEditor", "ComponentVisualizers", "SlateCore", "Slate","DesktopPlatform","UMG"});

		
	}
}
