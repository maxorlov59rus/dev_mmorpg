// Copyright BlackMatter Team. MMOCore plugin build rules (UE 5.8).
//
// Подключение внешних нативных зависимостей (Boost.Asio-заглушка и OpenSSL)
// для Win64 и Android-arm64. Реальные библиотеки подкладываются в
// Source/MMOCoreThirdParty/ (см. README ThirdParty): на боевом билде сюда
// добавляются реальные .lib/.a и include-пути, API модуля от этого не меняется.

using System;
using System.IO;
using UnrealBuildTool;

public class MMOCore : ModuleRules
{
	public MMOCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		bEnableExceptions = true; // Boost.Asio / OpenSSL могут бросать исключения

		// ------------------------------------------------------------------
		// Базовые зависимости движка: сокеты нам НЕ нужны (используем свой
		// POSIX/BSD-сокет слой), но Sockets нужен для IP-address хелперов.
		// ------------------------------------------------------------------
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"NetCommon",       // EAcceptConnection и сетевые enum'ы
			"Sockets",         // FIPv4Endpoint и утилиты адресов
			"Niagara",         // UEquipmentManagerComponent: NS_Enchant_* подложки (VFX)
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Projects",        // IPluginManager — получить путь плагина для ThirdParty
			"Json",            // конфиг endpoints в JSON
		});

		string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
		string ThirdPartyDir = Path.Combine(PluginDir, "Source", "MMOCoreThirdParty");

		// ------------------------------------------------------------------
		// WIN64
		// ------------------------------------------------------------------
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicIncludePaths.Add(Path.Combine(ThirdPartyDir, "boost_stub", "include"));
			PublicIncludePaths.Add(Path.Combine(ThirdPartyDir, "openssl_stub", "include"));

			// Заглушки header-only на этапе M0. Когда положим реальные сборки:
			//   PublicAdditionalLibraries.Add(Path.Combine(BoostDir, "lib", "win64", "libboost_system-x64-mt-1_83.lib"));
			//   PublicDelayLoadDLLs.Add("libcrypto-3-x64.dll");
			//   PublicDelayLoadDLLs.Add("libssl-3-x64.dll");
			RuntimeDependencies.Add(Path.Combine(ThirdPartyDir, "openssl_stub", "bin", "win64", "*.dll"));

			// Windows sockets
			PublicSystemLibraries.Add("ws2_32.lib");
		}

		// ------------------------------------------------------------------
		// ANDROID (arm64-v8a / x86_64)
		// ------------------------------------------------------------------
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicIncludePaths.Add(Path.Combine(ThirdPartyDir, "boost_stub", "include"));
			PublicIncludePaths.Add(Path.Combine(ThirdPartyDir, "openssl_stub", "include"));

			// NDK даёт нам libc++ (STL shared) и libz; OpenSSL/Boost собираются
			// отдельно через ExternalNativeLibraryBuild (см. п.4 ТЗ, libMMOCore.so).
			// Пример боевого подключения prebuilt .so/.a:
			//   string APPPLibFolder = Path.Combine(PluginDir, "Source", "MMOCore", "ThirdParty", "android");
			//   PublicAdditionalLibraries.Add(Path.Combine(APPPLibFolder, "arm64-v8a", "libcrypto.a"));
			//   PublicAdditionalLibraries.Add(Path.Combine(APPPLibFolder, "arm64-v8a", "libssl.a"));

			// Экспортируем символы нашего рантайма в Java-обёртку (JNI-мост при необходимости)
			PublicDefinitions.Add("MMOCORE_ANDROID_BUILD=1");

			// Указываем UBT, что нам нужна поставка native libs в APK:
			// (.so подхватятся автоматически из Intermediate/Android/arm64-v8a)
			// UE на Android линкуется с libc++_shared автоматически
		}

		// ------------------------------------------------------------------
		// LINUX (выделитель map/auth сервера в CI может гонять юнит-тесты)
		// ------------------------------------------------------------------
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicIncludePaths.Add(Path.Combine(ThirdPartyDir, "boost_stub", "include"));
			PublicIncludePaths.Add(Path.Combine(ThirdPartyDir, "openssl_stub", "include"));
			PublicSystemLibraries.Add("pthread");
			PublicSystemLibraries.Add("dl");
		}

		// ------------------------------------------------------------------
		// Модаккоды / debug
		// ------------------------------------------------------------------
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("MMOCORE_NETWORK_DEBUG=1");
			PrivateDependencyModuleNames.Add("TraceLog");
		}
		else
		{
			PublicDefinitions.Add("MMOCORE_NETWORK_DEBUG=0");
		}

		// Все определения — только для нашего модуля, чтобы не течь в другие
		PublicDefinitions.Add("MMOCORE_API=DLLEXPORT"); // переопределяется ниже корректно
		PublicDefinitions.RemoveAll(d => d == "MMOCORE_API=DLLEXPORT");
		// (MMOCORE_API задаётся через MMOCORE_API_HEADER в MMOCore.h — см. там же)
	}
}
