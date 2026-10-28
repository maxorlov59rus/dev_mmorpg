// Copyright BlackMatter Team. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMMOCore, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMMONet, Log, All);

class FMMOCoreModule : public IModuleInterface {
public:
    /** IModuleInterface */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Синглтон-доступ (валиден после StartupModule). */
    static inline FMMOCoreModule &Get() {
        return FModuleManager::LoadModuleChecked<FMMOCoreModule>(TEXT("MMOCore"));
    }

    static inline bool IsAvailable() {
        return FModuleManager::Get().IsModuleLoaded(TEXT("MMOCore"));
    }
};
