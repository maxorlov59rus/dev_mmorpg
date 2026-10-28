// Copyright BlackMatter Team. All Rights Reserved.

#include "MMOCoreEditor.h"
#include "MMOCore.h"
#include "MMOGameInstance.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "HAL/IConsoleManager.h"

static FAutoConsoleCommand CC_mmonetstatus(
	TEXT("mmo.netstatus"),
	TEXT("Показывает состояние MMOCore TCP-клиента (PIE/standalone)."),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (!GEngine || GEngine->GameInstances.Num() == 0)
		{
			UE_LOG(LogMMOCore, Display, TEXT("netstatus: no GameInstance yet"));
			return;
		}
		UGameInstance* GI = GEngine->GameInstances[0];
		AMMOGameInstance* MMOGI = Cast<AMMOGameInstance>(GI);
		if (!MMOGI)
		{
			UE_LOG(LogMMOCore, Display, TEXT("netstatus: active GameInstance is not AMMOGameInstance (%s)"),
				*GI->GetClass()->GetName());
			return;
		}
		const TSharedPtr<FMMONetworkClient> C = MMOGI->GetNetworkClient();
		if (!C.IsValid())
		{
			UE_LOG(LogMMOCore, Display, TEXT("netstatus: client not created"));
			return;
		}
		UE_LOG(LogMMOCore, Display,
			TEXT("netstatus: state=%d rtt=%.1fms uptime=%fs sent=%llu recv=%llu"),
			static_cast<int32>(C->GetState()),
			C->GetLastRoundTripMs(),
			C->GetUptime().GetTotalSeconds(),
			C->GetBytesSent(), C->GetBytesReceived());
	}));

void FMMOCoreEditorModule::StartupModule()
{
	UE_LOG(LogMMOCore, Display, TEXT("MMOCoreEditor module started"));
}

void FMMOCoreEditorModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FMMOCoreEditorModule, MMOCoreEditor)
