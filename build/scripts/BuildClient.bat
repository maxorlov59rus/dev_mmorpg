@echo off
setlocal EnableExtensions EnableDelayedExpansion
rem ============================================================================
rem  BuildClient.bat — UAT Wrapper для «Чёрная Материя» (UE5.8, Windows)
rem
rem  Унифицированная точка входа сборки клиента (ПК + Android).
rem  Использование:
rem    BuildClient.bat --platforms=Win64+Android --config=Development [--clean]
rem                    [--cook-only] [--architectures=Win64:x64_17;Android:arm64]
rem                    [--ue=C:\Program Files\Epic Games\UE_5.8] [--nosplit]
rem
rem  Шаги:
rem    0. Парсинг аргументов, определение путей.
rem    1. Очистка Intermediate/Saved/Binaries (при --clean).
rem    2. Компиляция C++ части через UnrealBuildTool (быстрый путь без коока).
rem    3. Cook только запрошенных платформ (-run=BuildCookRun -cookflavor).
rem    4. Stage/Package/Pak -> Packaged\<Platform>.
rem ============================================================================

set "PROJECT_DIR=%~dp0..\.."
for %%I in ("%PROJECT_DIR%") do set "PROJECT_DIR=%%~fI"
set "UPROJECT=%PROJECT_DIR%\DMClient.uproject"

set "UE_ROOT=C:\Program Files\Epic Games\UE_5.8"
set "PLATFORMS=Win64+Android"
set "CLIENT_CONFIG=Development"
set "ARCH_ARGS="
set "DO_CLEAN=0"
set "COOK_ONLY=0"
set "EXTRA_ARGS="

rem --------------------------- парсинг аргументов ----------------------------
:parse_args
if "%~1"=="" goto args_done
for /f "tokens=1,* delims==" %%A in ("%~1") do (
  set "KEY=%%~A" & set "VAL=%%~B"
)
if /i "%KEY%"=="--platforms"     set "PLATFORMS=%VAL%"
if /i "%KEY%"=="--config"        set "CLIENT_CONFIG=%VAL%"
if /i "%KEY%"=="--ue"            set "UE_ROOT=%VAL%"
if /i "%KEY%"=="--architectures" set "ARCH_ARGS=-architectures=%VAL%"
if /i "%~1"=="--clean"           set "DO_CLEAN=1"
if /i "%~1"=="--cook-only"       set "COOK_ONLY=1"
shift
goto parse_args
:args_done

set "RUNUAT=%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat"
set "UBT=%UE_ROOT%\Engine\Binaries\ThirdParty\DotNet\*\win-x64\dotnet.exe"
set "UBTOOL=%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll"

if not exist "%RUNUAT%" (
  echo [FATAL] RunUAT.bat не найден: "%RUNUAT%" 1>&2
  echo         Укажите корректный --ue=... 1>&2
  exit /b 2
)
if not exist "%UPROJECT%" (
  echo [FATAL] uproject не найден: "%UPROJECT%" 1>&2
  exit /b 2
)

echo ============================================================
echo  BlackMatter client build
echo   UE_ROOT   : %UE_ROOT%
echo   UPROJECT  : %UPROJECT%
echo   PLATFORMS : %PLATFORMS%
echo   CONFIG    : %CLIENT_CONFIG%
echo   CLEAN     : %DO_CLEAN%   COOK_ONLY: %COOK_ONLY%
echo ============================================================

rem ------------------------------ шаг 1: clean -------------------------------
if "%DO_CLEAN%"=="1" (
  echo [1/4] Очистка Intermediate / Saved / Binaries ...
  for %%D in ("Intermediate" "Saved" "Binaries") do (
    if exist "%PROJECT_DIR%\%%~D" rd /s /q "%PROJECT_DIR%\%%~D"
  )
  rem Чистим так же бинарники плагина MMOCore (иначе stale obj ломают линковку)
  for /d %%P in ("%PROJECT_DIR%\Plugins\*") do (
    for %%D in ("Intermediate" "Binaries" "Saved") do (
      if exist "%%P\%%~D" rd /s /q "%%P\%%~D"
    )
  )
) else (
  echo [1/4] Пропускаю очистку ^(без флага --clean^)
)

rem --------------------- шаг 2: компиляция C++ (UBT) -------------------------
echo [2/4] Компиляция C++ модулей (UnrealBuildTool)^...
call "%RUNUAT%" BuildCookRun ^
  -project="%UPROJECT%" ^
  -target=DMClient ^
  -nobuilduat ^
  -build -nocodeeditor ^
  -utf8output -NoP4 -waitmutex %ARCH_ARGS%
if errorlevel 1 (
  echo [FATAL] Компиляция C++ упала — cook не запускаем. 1>&2
  exit /b 1
)

rem ----------------------- шаг 3+4: cook/stage/package -----------------------
if "%COOK_ONLY%"=="1" (
  echo [3/4] Только Cook (без стейджа/пакета)^...
  call "%RUNUAT%" BuildCookRun ^
    -project="%UPROJECT%" ^
    -platforms=%PLATFORMS% ^
    -clientconfig=%CLIENT_CONFIG% ^
    -cook -unversioned -compressed -fromfile -iterate ^
    -utf8output -NoP4 -waitmutex %ARCH_ARGS%
) else (
  echo [3/4] Cook + [4/4] Stage/Package/Pak для: %PLATFORMS%
  call "%RUNUAT%" BuildCookRun ^
    -project="%UPROJECT%" ^
    -platforms=%PLATFORMS% ^
    -clientconfig=%CLIENT_CONFIG% ^
    -cook -stage -package -pak -iostore ^
    -archivedirectory="%PROJECT_DIR%\Packaged" ^
    -utf8output -NoP4 -waitmutex -nocompressedincookedsizes %ARCH_ARGS% %EXTRA_ARGS%
)
if errorlevel 1 (
  echo [FATAL] BuildCookRun завершился с ошибкой. См. лог выше. 1>&2
  exit /b 1
)

echo.
echo [OK] Клиент собран: %PROJECT_DIR%\Packaged
endlocal
exit /b 0
