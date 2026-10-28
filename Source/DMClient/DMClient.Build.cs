// Copyright BlackMatter Team. DMClient runtime module build rules (UE 5.8).

using UnrealBuildTool;

public class DMClient : ModuleRules
{
	public DMClient(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"CommonUI",
			"ModularGameplay",
			"GameplayAbilities",
			"NetCommon",
			// Сетевой клиент живёт в плагине — модуль проекта только дергает его API.
			"MMOCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
		});

		if (Target.Type == TargetType.Editor)
		{
			// Только редакторные цели: превью dbc-таблиц, валидация opcodes.defs.
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
