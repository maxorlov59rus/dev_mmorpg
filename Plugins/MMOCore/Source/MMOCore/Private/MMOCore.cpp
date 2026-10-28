// Copyright BlackMatter Team. All Rights Reserved.

#include "MMOCore.h"

#include "Logging/LogMacros.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"

DEFINE_LOG_CATEGORY(LogMMOCore);
DEFINE_LOG_CATEGORY(LogMMONet);

void FMMOCoreModule::StartupModule()
{
	// Места поиска ThirdParty регистрировать здесь, если понадобится
	// динамическая загрузка DLL/SO (DelayLoad на Win64).
	UE_LOG(LogMMOCore, Display, TEXT("MMOCore %s startup (%s)"),
		ANSI_TO_TCHAR("0.1.0"), FApp::GetProjectName());
}

void FMMOCoreModule::ShutdownModule()
{
	UE_LOG(LogMMOCore, Display, TEXT("MMOCore shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMMOCoreModule, MMOCore)
