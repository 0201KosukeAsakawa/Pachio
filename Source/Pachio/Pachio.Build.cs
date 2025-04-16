// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pachio : ModuleRules
{
	public Pachio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

        PublicDependencyModuleNames.AddRange(new string[] { "CableComponent" });
    }
}
