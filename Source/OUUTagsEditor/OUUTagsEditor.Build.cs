using UnrealBuildTool;

public class OUUTagsEditor : ModuleRules
{
	public OUUTagsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		IWYUSupport = IWYUSupport.Full;

#if UE_5_4_OR_LATER
		bWarningsAsErrors = true;
#endif
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"OUUTags",
			"GameplayTags",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"MessageLog",
			"GameplayTagsEditor",
			"SlateCore",
			"Slate",
			"InputCore",
			"EditorSubsystem",
		});
	}
}