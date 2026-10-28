// Copyright BlackMatter Team.

#pragma once

#include "CoreMinimal.h"
#include "DMClient.h" // DMClientBuild::* — проверка совпадения протокола при загрузке редактора

DECLARE_LOG_CATEGORY_EXTERN(LogDMClientEditor, Log, All);

class FDMClientEditorModule : public IModuleInterface {
public:
    // В M1 здесь регистрируется ToolMenu-секция «Black Matter → Validate opcodes.defs»
    // и DataTable→dbc exporter (пайплайн docs/04 §3).
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
