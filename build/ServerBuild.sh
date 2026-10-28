#!/usr/bin/env bash
# ============================================================================
#  ServerBuild.sh — CMake wrapper для серверного ядра «Чёрная Материя»
#  (dm-authserver / dm-loginserver / dm-worldserver / dm-mapserver)
#
#  Единая точка входа CI и локальной разработки (Linux/macOS; Windows — .bat).
#
#  Использование:
#    ./ServerBuild.sh --config=Release [--ccache] [--clean] [--jobs=N]
#                     [--source=DIR] [--build=DIR] [--tests=ON|OFF] [--install]
#
#  Логика:
#    1. Проверка зависимостей (cmake, ninja/make, компилятор, Boost, OpenSSL,
#       MySQL-клиент, Lua) — падение ДО configure с понятным сообщением.
#    2. Настройка ccache (если запрошен и доступен).
#    3. cmake configure + build + (опционально) test/install.
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# ----------------------------- дефолты --------------------------------------
CONFIG="Release"
USE_CCACHE=0
DO_CLEAN=0
JOBS="$( (nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) )"
SOURCE_DIR="${DM_SOURCE_DIR:-${REPO_ROOT}/server}"
BUILD_DIR="${DM_BUILD_DIR:-${SOURCE_DIR}/build-linux}"
GENERATOR="${DM_CMAKE_GENERATOR:-Ninja}"
WITH_TESTS="ON"
DO_INSTALL=0

# ----------------------------- аргументы ------------------------------------
for arg in "$@"; do
  case "$arg" in
    --config=*)   CONFIG="${arg#*=}" ;;
    --ccache)     USE_CCACHE=1 ;;
    --clean)      DO_CLEAN=1 ;;
    --jobs=*)     JOBS="${arg#*=}" ;;
    --source=*)   SOURCE_DIR="${arg#*=}" ;;
    --build=*)    BUILD_DIR="${arg#*=}" ;;
    --generator=*) GENERATOR="${arg#*=}" ;;
    --tests=*)    WITH_TESTS="${arg#*=}" ;;
    --install)    DO_INSTALL=1 ;;
    -h|--help)    grep '^#  ' "$0" | sed 's/^#  //'; exit 0 ;;
    *) echo "[FATAL] неизвестный аргумент: $arg" >&2; exit 2 ;;
  esac
done

log()  { printf '\033[1;34m[server-build]\033[0m %s\n' "$*"; }
die()  { printf '\033[1;31m[FATAL]\033[0m %s\n' "$*" >&2; exit 1; }

# ------------------------ шаг 0: проверка зависимостей -----------------------
log "Проверка тулчейна (config=${CONFIG}, generator=${GENERATOR})..."

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "не найдена утилита '$1'. Установка: $2"
}
need_cmd cmake  "apt-get install -y cmake  (>=3.24)"
need_cmd "${CXX:-g++}" "apt-get install -y g++-12"

if [[ "${GENERATOR}" == "Ninja" ]]; then
  need_cmd ninja "apt-get install -y ninja-build"
else
  need_cmd make "apt-get install -y make"
fi

# Заголовочные зависимости TrinityCore-like стека
check_header() {
  local hdr="$1" hint="$2"
  echo -e "#include <$hdr>\nint main(){return 0;}" | \
    "${CXX:-c++}" -x c++ -std=c++20 -I/usr/include -fsyntax-only - >/dev/null 2>&1 \
    || die "заголовок <$hdr> не найден. Установка: $hint"
}
check_header boost/asio.hpp        "apt-get install -y libboost-all-dev"
check_header openssl/evp.h         "apt-get install -y libssl-dev"
check_header mysql/mysql.h         "apt-get install -y libmysqlclient-dev"
check_header lua.h                 "apt-get install -y liblua5.4-dev"

[[ -f "${SOURCE_DIR}/CMakeLists.txt" ]] || die "нет ${SOURCE_DIR}/CMakeLists.txt"
log "Зависимости OK."

# --------------------------- шаг 1: ccache -----------------------------------
CMAKE_EXTRA=()
if [[ "${USE_CCACHE}" == "1" ]]; then
  if command -v ccache >/dev/null 2>&1; then
    export CCACHE_BASEDIR="${REPO_ROOT}"
    export CCACHE_DIR="${CCACHE_DIR:-${REPO_ROOT}/.cache/ccache}"
    export CCACHE_COMPILERCHECK=content
    ccache --max-size=4G >/dev/null
    CMAKE_EXTRA+=("-DCMAKE_C_COMPILER_LAUNCHER=ccache"
                  "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache")
    log "ccache включён: dir=${CCACHE_DIR}"
  else
    log "ВНИМАНИЕ: --ccache запрошен, но ccache не установлен — игнорирую."
  fi
fi

# --------------------------- шаг 2: clean ------------------------------------
if [[ "${DO_CLEAN}" == "1" && -d "${BUILD_DIR}" ]]; then
  log "Удаление ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
fi
mkdir -p "${BUILD_DIR}"

# --------------------------- шаг 3: configure --------------------------------
log "CMake configure -> ${BUILD_DIR}"
cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" \
  -G "${GENERATOR}" \
  -DCMAKE_BUILD_TYPE="${CONFIG}" \
  -DDM_BUILD_TESTS="${WITH_TESTS}" \
  -DDM_SCRIPTING_LUA=ON \
  -DDM_PROTOCOL_DEFS="${REPO_ROOT}/proto/opcodes.defs" \
  "${CMAKE_EXTRA[@]}" \
  ${DM_EXTRA_CMAKE_FLAGS:-}

# ----------------------------- шаг 4: build ----------------------------------
log "Сборка -j${JOBS} ..."
cmake --build "${BUILD_DIR}" --config "${CONFIG}" -j "${JOBS}" -- \
  ${GENERATOR:+}  # Ninja/Make принимают стандартные флаги

BIN_DIR="${BUILD_DIR}/bin"
log "Бинарники:"
ls -1 "${BIN_DIR}" 2>/dev/null | sed 's/^/    /' || log "  (bin пуст — проверьте target'ы CMake)"

# ------------------------- шаг 5: тесты/установка ----------------------------
if [[ "${WITH_TESTS}" == "ON" ]]; then
  log "ctest (smoke, без долгих integration)..."
  ctest --test-dir "${BUILD_DIR}" --output-on-failure \
        -L smoke -j "${JOBS}" || die "часть smoke-тестов упала"
fi
if [[ "${DO_INSTALL}" == "1" ]]; then
  cmake --install "${BUILD_DIR}" --prefix "${DM_INSTALL_PREFIX:-${REPO_ROOT}/dist/server-${CONFIG}}"
  log "Установлено в ${DM_INSTALL_PREFIX:-${REPO_ROOT}/dist/server-${CONFIG}}"
fi

log "Готово. Конфиг=${CONFIG}, время: $(date '+%H:%M:%S')"
