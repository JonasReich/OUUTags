// Copyright (c) 2025 Jonas Reich & Contributors

using UnrealBuildTool;

public class OUUTags : ModuleRules
{
	public OUUTags(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		IWYUSupport = IWYUSupport.Full;

#if UE_5_4_OR_LATER
		bWarningsAsErrors = true;
#endif

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("GameplayTagsEditor");
		}
	}
}