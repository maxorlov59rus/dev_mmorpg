// Copyright BlackMatter Team. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FMMOCoreEditorModule : public IModuleInterface {
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
