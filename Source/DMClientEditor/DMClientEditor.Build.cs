// Copyright BlackMatter Team. Editor module build rules (UE 5.8).

using UnrealBuildTool;

public class DMClientEditor : ModuleRules
{
	public DMClientEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"DMClient",
			"MMOCore",        // доступ к FMMOOpcode для валидатора opcodes.defs
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"ToolMenus",
			"Slate",
			"SlateCore",
			"AssetTools",
			"DeveloperSettings",
		});
	}
}
