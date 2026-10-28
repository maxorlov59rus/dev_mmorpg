@echo off
setlocal EnableExtensions EnableDelayedExpansion
rem ============================================================================
rem  ServerBuild.bat — CMake/MSBuild wrapper для сервера «Чёрная Материя» (Windows)
rem
rem  Использование:
rem    ServerBuild.bat --config=Release [--clcache] [--clean] [--generator="..."]
rem
rem  Логика аналогична ServerBuild.sh: проверка зависимостей -> clcache ->
rem  cmake configure -> msbuild. NDK/Android тут НЕ нужен (только x64 сервер).
rem ============================================================================

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "REPO_ROOT=%%~fI"

set "CONFIG=Release"
set "USE_CLCACHE=0"
set "DO_CLEAN=0"
set "SOURCE_DIR=%REPO_ROOT%\dm-server"
set "BUILD_DIR=%SOURCE_DIR%\build-win"
set "GENERATOR=Visual Studio 17 2022"

:parse
if "%~1"=="" goto parsed
for /f "tokens=1,* delims==" %%A in ("%~1") do (set "K=%%~A" & set "V=%%~B")
if /i "!K!"=="--config"     set "CONFIG=!V!"
if /i "!K!"=="--clcache"    set "USE_CLCACHE=1"
if /i "!K!"=="--clean"      set "DO_CLEAN=1"
if /i "!K!"=="--source"     set "SOURCE_DIR=!V!"
if /i "!K!"=="--build"      set "BUILD_DIR=!V!"
if /i "!K!"=="--generator"  set "GENERATOR=!V!"
shift & goto parse
:parsed

echo [server-build] config=%CONFIG% generator="%GENERATOR%"

rem ------------------------- шаг 0: зависимости ------------------------------
where cmake >nul 2>&1 || (echo [FATAL] cmake не в PATH 1>&2 & exit /b 1)
if not exist "%SOURCE_DIR%\CMakeLists.txt" (
  echo [FATAL] нет %SOURCE_DIR%\CMakeLists.txt 1>&2 & exit /b 1
)

rem Boost/OpenSSL через vcpkg (канон для TRC-like на Windows)
if not defined VCPKG_ROOT (
  if exist "C:\vcpkg\vcpkg.exe" set "VCPKG_ROOT=C:\vcpkg"
)
if defined VCPKG_ROOT (
  set "TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
) else (
  echo [WARN] VCPKG_ROOT не задан — ожидаю Boost/OpenSSL/MySQL в системных путях
)

rem --------------------------- шаг 1: clcache ---------------------------------
set "LAUNCHER="
if "%USE_CLCACHE%"=="1" (
  where clcache >nul 2>&1 && (
    set "CLCACHE_DIR=%REPO_ROOT%\.cache\clcache"
    set "LAUNCHER=-DCMAKE_VS_GLOBALS=CLToolExe=clcache.exe -DCMAKE_VS_GLOBALS=CLToolPath=%CLCACHE_DIR%"
    echo [server-build] clcache включён: !CLCACHE_DIR!
  ) || echo [server-build] WARN: clcache не найден, игнорирую --clcache
)

rem ---------------------------- шаг 2: clean ----------------------------------
if "%DO_CLEAN%"=="1" if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

rem -------------------------- шаг 3: configure --------------------------------
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "%GENERATOR%" -A x64 ^
  -DDM_BUILD_TESTS=ON -DDM_SCRIPTING_LUA=ON ^
  -DDM_PROTOCOL_DEFS="%REPO_ROOT%\proto\opcodes.defs" ^
  %TOOLCHAIN% %LAUNCHER%
if errorlevel 1 (echo [FATAL] configure упал 1>&2 & exit /b 1)

rem ----------------------------- шаг 4: build ---------------------------------
cmake --build "%BUILD_DIR%" --config %CONFIG% --parallel -- -nologo
if errorlevel 1 (echo [FATAL] сборка упала 1>&2 & exit /b 1)

echo [OK] Сервер собран: %BUILD_DIR%\%CONFIG%
endlocal & exit /b 0
