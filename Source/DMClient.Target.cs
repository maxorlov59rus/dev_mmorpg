// Copyright BlackMatter Team. Black matter MMORPG client — runtime target (UE 5.8).

using UnrealBuildTool;
using System.Collections.Generic;

public class DMClientTarget : TargetRules
{
	public DMClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;      // UE 5.5+ актуальный дефолт
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bUseUnity = false;                                   // см. ADR-004 в DMClient.uproject
		CppStandard = CppStandardVersion.Cpp20;              // единый стандарт клиент↔сервер (dm-server: C++20)

		ExtraModuleNames.AddRange(new string[] { "DMClient" });
	}
}
