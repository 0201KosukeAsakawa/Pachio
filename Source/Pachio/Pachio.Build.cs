using UnrealBuildTool;

public class Pachio : ModuleRules
{
    public Pachio(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "CableComponent",
            "Json",
            "JsonUtilities",
            "FMODStudio",
            "Niagara"   ,
            "ProceduralMeshComponent",
            "UE5Coro",
        });
    }
}



