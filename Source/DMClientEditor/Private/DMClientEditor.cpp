// Copyright BlackMatter Team.

#include "DMClientEditor.h"

DEFINE_LOG_CATEGORY(LogDMClientEditor);

void FDMClientEditorModule::StartupModule()
{
	UE_LOG(LogDMClientEditor, Display, TEXT("DMClientEditor loaded (protocol v%d)."),
		DMClientBuild::ProtocolVersion);
}

void FDMClientEditorModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FDMClientEditorModule, DMClientEditor)
