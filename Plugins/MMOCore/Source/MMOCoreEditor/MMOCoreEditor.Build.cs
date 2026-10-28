// Copyright BlackMatter Team. All Rights Reserved.

using UnrealBuildTool;

public class MMOCoreEditor : ModuleRules
{
	public MMOCoreEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PModulePCHUsage.UseSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"MMOCore", // консольные команды должны видеть рантайм-классы
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Engine",
			"Slate",
			"SlateCore",
			"UnrealEd",
			"DeveloperSettings",
		});
	}
}
