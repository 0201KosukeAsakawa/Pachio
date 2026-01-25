// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO; // ’Ç‰Á
using UnrealBuildTool;

public class RenderingExt : ModuleRules
{
	public RenderingExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
            new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RenderCore",
				"RHI",
                "Renderer"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
            new string[]
			{
				"Projects",
				"ApplicationCore",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
    }
}
