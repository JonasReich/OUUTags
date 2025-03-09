// Copyright (c) 2025 Jonas Reich & Contributors

using UnrealBuildTool;

public class OUUTagsTests : ModuleRules
{
    public OUUTagsTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "OUUTags",
                "GameplayTags"
            }
        );
    }
}
