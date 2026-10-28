// Copyright BlackMatter Team.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDMClient, Log, All);

/**
 * Идентификаторы текущей сборки клиента. Заполняются UBT-define'ами
 * (см. DMClient.Build.cs — сюда добавим version header из CI), пока захардкожено.
 */
namespace DMClientBuild {
inline constexpr int32 ProtocolVersion =
    2; // синхронно с `@version 2` в proto/opcodes.defs (additive: репутация/ренегаты v1.2)
inline constexpr const TCHAR *ProjectVersion = TEXT("0.1.0");
} // namespace DMClientBuild
