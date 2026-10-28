// Copyright BlackMatter Team.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDMClient, Log, All);

/**
 * Идентификаторы текущей сборки клиента. Заполняются UBT-define'ами
 * (см. DMClient.Build.cs — сюда добавим version header из CI), пока захардкожено.
 */
namespace DMClientBuild
{
	inline constexpr int32 ProtocolVersion = 1;   // синхронно с `@version 1` в proto/opcodes.defs (единый источник истины)
	inline constexpr const TCHAR* ProjectVersion = TEXT("0.1.0");
}
