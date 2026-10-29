// Copyright BlackMatter Team. Black matter MMORPG client — editor target (UE 5.8).

using UnrealBuildTool;
using System.Collections.Generic;

public class DMClientEditorTarget : TargetRules
{
	public DMClientEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bUseUnityBuild = false;
		CppStandard = CppStandardVersion.Cpp20;

		// Пакетирование Android-билда из редактора (Cook + stage) — без лишних модулей.
		// BuildEnvironment по умолчанию Shared (установленный движок не поддерживает Unique).

		ExtraModuleNames.AddRange(new string[] { "DMClient", "DMClientEditor" });
	}
}
