#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BuildOrchestrator.py — единый оркестратор параллельной сборки «Чёрная Материя».

Компонент А (клиент UE5.8: Win64 + Android) и Компонент Б (сервер C++ TrinityCore-like:
Linux GCC/Clang + Windows MSVC) собираются ОДНОВРЕМЕННО через asyncio.subprocess.

Логика:
  1. Чтение параметров из build_config.json (пути к движку, ветки, флаги).
  2. Спин-ап задач: UE5 Build Agent, Linux Server Agent, Windows Server Agent.
  3. asyncio.gather(...) — ожидание всех задач.
  4. Fail Fast: первая упавшая задача отменяет оставшиеся, печатаются хвосты логов ВСЕХ задач.
  5. Если все успешны — этап Packaging (сборка артефактов + release_manifest.json).

Запуск:
  python BuildOrchestrator.py --config build/build_config.json --targets all
  python BuildOrchestrator.py --targets client,server_linux --clean
"""

from __future__ import annotations

import argparse
import asyncio
import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Optional

IS_WINDOWS = os.name == "nt"


# --------------------------------------------------------------------------- #
#  Конфигурация                                                               #
# --------------------------------------------------------------------------- #

DEFAULT_CONFIG_PATH = Path(__file__).resolve().parent / "build_config.json"


def load_config(path: Path) -> dict[str, Any]:
    if not path.exists():
        die(f"Конфиг не найден: {path}")
    with path.open("r", encoding="utf-8") as fh:
        cfg = json.load(fh)
    validate_config(cfg)
    return cfg


REQUIRED_KEYS = (
    ("project", "uproject"),
    ("engines", "ue_root"),
    ("engines", "runuat"),
    ("targets", "client"),
    ("targets", "server_linux"),
    ("targets", "server_windows"),
    ("pipeline",),
)


def validate_config(cfg: dict[str, Any]) -> None:
    missing = []
    for keys in REQUIRED_KEYS:
        node: Any = cfg
        for k in keys:
            if not isinstance(node, dict) or k not in node:
                missing.append(".".join(keys))
                break
            node = node[k]
    if missing:
        die("build_config.json неполный, отсутствуют секции: " + ", ".join(missing))


# --------------------------------------------------------------------------- #
#  Модель задачи сборки                                                        #
# --------------------------------------------------------------------------- #


@dataclass
class BuildTask:
    """Описание одной параллельной сборки (агент + команда + лог)."""

    name: str                       # "client" | "server_linux" | "server_windows"
    command: list[str]              # argv
    cwd: Path                       # рабочая директория агента (локальная симуляция)
    env: dict[str, str] = field(default_factory=dict)
    log_path: Optional[Path] = None
    started_at: float = 0.0
    finished_at: float = 0.0
    exit_code: Optional[int] = None
    cancelled: bool = False

    @property
    def duration(self) -> float:
        if self.started_at and self.finished_at:
            return self.finished_at - self.started_at
        return 0.0

    @property
    def status(self) -> str:
        if self.cancelled:
            return "CANCELLED"
        if self.exit_code is None:
            return "RUNNING"
        return "OK" if self.exit_code == 0 else f"FAILED({self.exit_code})"


class TaskFailure(Exception):
    def __init__(self, task: BuildTask):
        super().__init__(f"task '{task.name}' failed with exit code {task.exit_code}")
        self.task = task


# --------------------------------------------------------------------------- #
#  Построение команд                                                           #
# --------------------------------------------------------------------------- #


def _tail_file(path: Optional[Path], lines: int = 60) -> str:
    if not path or not Path(path).exists():
        return "<лог отсутствует>"
    try:
        with Path(path).open("r", encoding="utf-8", errors="replace") as fh:
            data = fh.readlines()
        return "".join(data[-lines:])
    except OSError as exc:
        return f"<не удалось прочитать лог: {exc}>"


def build_client_command(cfg: dict[str, Any], clean: bool) -> BuildTask:
    """UE5 Build Agent: RunUAT.bat BuildCookRun -platform=Win64+Android."""
    engines = cfg["engines"]
    project = cfg["project"]
    tgt = cfg["targets"]["client"]

    runuat = Path(engines["runuat"])
    uproject = Path(project.get("repository", ".")) / project["uproject"]

    platforms = "+".join(tgt["platforms"])
    configs = tgt.get("configs", {})
    config = configs.get(tgt["platforms"][0], "Development")

    cmd = [
        str(runuat), "BuildCookRun",
        f"-project={uproject}",
        f"-platforms={platforms}",
        f"-clientconfig={config}",
        "-build",
        "-cook",
        "-stage",
        "-package",
        "-pak",
        f"-archive={tgt['archive_dir']}",
        f"-mappack={'true' if tgt.get('mappack') else 'false'}",
    ]
    if clean:
        cmd.append("-clean")
    cmd += tgt.get("extra_uat_args", [])

    workdir = Path(cfg["agents"]["client"]["workdir"]) if "agents" in cfg else Path.cwd()
    return BuildTask(
        name="client",
        command=cmd,
        cwd=workdir,
        env={"DM_BRANCH": project.get("branch_client", "master")},
    )


def build_server_linux_command(cfg: dict[str, Any], clean: bool) -> BuildTask:
    """Linux Server Build Agent: ./ServerBuild.sh --config=Release."""
    tgt = cfg["targets"]["server_linux"]
    repo = Path(cfg["project"].get("repository", "."))
    script = repo / "build" / "ServerBuild.sh"

    cmd = ["bash", str(script), "--config=Release"]
    if clean:
        cmd.append("--clean")
    if tgt.get("use_ccache", True):
        cmd.append("--ccache")

    env = {
        "CC": tgt.get("cc_compiler", "gcc"),
        "CXX": tgt.get("cxx_compiler", "g++"),
        "DM_CMAKE_GENERATOR": tgt.get("generator", "Ninja"),
        "DM_SOURCE_DIR": str(repo / tgt["source_dir"]),
        "DM_BUILD_DIR": str(repo / tgt["build_dir"]),
        "DM_BRANCH": cfg["project"].get("branch_server", "master"),
    }
    return BuildTask(name="server_linux", command=cmd, cwd=repo, env=env)


def build_server_windows_command(cfg: dict[str, Any], clean: bool) -> BuildTask:
    """Windows Server Build Agent: msbuild Server.sln /p:Configuration=Release."""
    tgt = cfg["targets"]["server_windows"]
    repo = Path(cfg["project"].get("repository", "."))
    sln = repo / tgt["solution"]

    cmd = ["msbuild", str(sln)] + tgt.get("msbuild_flags", ["/p:Configuration=Release"])
    if clean:
        cmd.append("/t:Clean")
    cmd.append("/t:Build")

    env = {
        "DM_CLCACHE": "1" if tgt.get("use_clcache", True) else "0",
        "DM_BUILD_DIR": str(repo / tgt["build_dir"]),
        "DM_BRANCH": cfg["project"].get("branch_server", "master"),
    }
    return BuildTask(name="server_windows", command=cmd, cwd=repo, env=env)


TASK_BUILDERS = {
    "client": build_client_command,
    "server_linux": build_server_linux_command,
    "server_windows": build_server_windows_command,
}


# --------------------------------------------------------------------------- #
#  Исполнение                                                                  #
# --------------------------------------------------------------------------- #


async def run_task(task: BuildTask, log_dir: Path, timeout: float) -> BuildTask:
    """Запуск одной сборки: stdout/stderr → файл лога агента + консоль оркестратора."""
    log_dir.mkdir(parents=True, exist_ok=True)
    ts = datetime.now().strftime("%Y%m%d-%H%M%S")
    task.log_path = log_dir / f"{task.name}_{ts}.log"
    task.env = {**os.environ, **task.env, "PYTHONUNBUFFERED": "1"}

    banner(f"[{task.name}] spawn: {' '.join(task.command)}")
    task.started_at = time.monotonic()

    with task.log_path.open("w", encoding="utf-8") as log_fh:
        log_fh.write(f"# agent task: {task.name}\n# cmd: {' '.join(task.command)}\n")
        log_fh.flush()
        try:
            proc = await asyncio.create_subprocess_exec(
                *task.command,
                cwd=str(task.cwd),
                env=task.env,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.STDOUT,
            )
        except (FileNotFoundError, NotADirectoryError) as exc:
            # Агент недоступен / бинарь не в PATH — считаем это падением задачи,
            # а не оркестратора (важно для fail-fast логики ниже).
            log_fh.write(f"\n[spawn error] {exc}\n")
            task.exit_code = 127
            task.finished_at = time.monotonic()
            raise TaskFailure(task) from exc

        async def pump() -> None:
            assert proc.stdout is not None
            while True:
                line = await proc.stdout.readline()
                if not line:
                    break
                text = line.decode("utf-8", errors="replace").rstrip("\r\n")
                log_fh.write(text + "\n")
                log_fh.flush()
                print(f"  [{task.name[:12]}|{int(time.monotonic()-task.started_at):>5}s] {text[:240]}")

        try:
            await asyncio.wait_for(pump(), timeout=timeout)
            task.exit_code = await asyncio.wait_for(proc.wait(), timeout=30)
        except asyncio.TimeoutError:
            banner(f"[{task.name}] TIMEOUT {timeout}s — kill")
            kill_tree(proc)
            task.exit_code = 124
        except asyncio.CancelledError:
            banner(f"[{task.name}] CANCELLED by fail-fast — kill")
            kill_tree(proc)
            # Дожидаемся фактического завершения процесса, иначе transport
            # закроется после остановки event loop (RuntimeError в логах CI).
            try:
                await asyncio.shield(asyncio.wait_for(proc.wait(), timeout=10))
            except (asyncio.TimeoutError, asyncio.CancelledError, OSError):
                pass
            task.cancelled = True
            task.exit_code = None
            raise
        finally:
            task.finished_at = time.monotonic()

    if task.exit_code != 0:
        raise TaskFailure(task)
    banner(f"[{task.name}] OK ({task.duration:.1f}s)")
    return task


def kill_tree(proc: asyncio.subprocess.Process) -> None:
    """Убить процесс и детей (UAT/msbuild порождают дочерние компиляторы)."""
    try:
        if IS_WINDOWS:
            subprocess.run(
                ["taskkill", "/F", "/T", "/PID", str(proc.pid)],
                capture_output=True, check=False,
            )
        else:
            proc.kill()
    except ProcessLookupError:
        pass


# --------------------------------------------------------------------------- #
#  Fail-fast gather                                                            #
# --------------------------------------------------------------------------- #


async def run_pipeline(tasks: list[BuildTask], log_dir: Path, timeout: float,
                       fail_fast: bool) -> list[BuildTask]:
    coros = [asyncio.ensure_future(run_task(t, log_dir, timeout)) for t in tasks]
    if fail_fast:
        done, pending = await asyncio.wait(coros, return_when=asyncio.FIRST_EXCEPTION)
        first_error: Optional[BaseException] = None
        for fut in done:
            if fut.exception() and not first_error:
                first_error = fut.exception()
        if first_error:
            for fut in pending:
                fut.cancel()
            await asyncio.gather(*pending, return_exceptions=True)
            raise first_error
        await asyncio.gather(*done)   # пробросить исключение, если всё же есть
        return tasks

    results = await asyncio.gather(*coros, return_exceptions=True)
    errors = [r for r in results if isinstance(r, BaseException)]
    if errors:
        raise errors[0]
    return tasks


# --------------------------------------------------------------------------- #
#  Packaging                                                                   #
# --------------------------------------------------------------------------- #


def sha256_of(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def package_artifacts(cfg: dict[str, Any], tasks: list[BuildTask]) -> Path:
    """Этап упаковки: собираем выходы билдов в artifacts/<ts>/ + манифест."""
    pkg_cfg = cfg.get("packaging", {})
    if not pkg_cfg.get("enabled", True):
        banner("packaging.enabled=false — пропускаю этап упаковки")
        return Path(".")

    repo = Path(cfg["project"].get("repository", "."))
    ts = datetime.now().strftime("%Y%m%d-%H%M%S")
    out = repo / pkg_cfg.get("output_dir", "artifacts") / ts
    out.mkdir(parents=True, exist_ok=True)

    manifest: dict[str, Any] = {
        "project": cfg["project"]["name"],
        "timestamp_utc": datetime.now(timezone.utc).isoformat(),
        "branch_client": cfg["project"].get("branch_client"),
        "branch_server": cfg["project"].get("branch_server"),
        "tasks": [],
        "artifacts": [],
    }

    for t in tasks:
        manifest["tasks"].append({
            "name": t.name,
            "status": t.status,
            "duration_s": round(t.duration, 1),
            "log": str(t.log_path) if t.log_path else None,
        })
        candidates = {
            "client": repo / "Packaged",
            "server_linux": Path(t.env["DM_BUILD_DIR"]) / "bin" if t.env.get("DM_BUILD_DIR") else None,
            "server_windows": Path(t.env["DM_BUILD_DIR"]) / "Release" if t.env.get("DM_BUILD_DIR") else None,
        }.get(t.name)
        if candidates and Path(candidates).exists():
            dst = out / t.name
            shutil.copytree(candidates, dst, dirs_exist_ok=True)
            for f in dst.rglob("*"):
                if f.is_file():
                    manifest["artifacts"].append({
                        "task": t.name,
                        "path": str(f.relative_to(out)),
                        "sha256": sha256_of(f),
                    })

    manifest_path = out / pkg_cfg.get("manifest_name", "release_manifest.json")
    manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2),
                             encoding="utf-8")
    banner(f"Packaging готов: {out}  (артефактов: {len(manifest['artifacts'])})")
    return out


# --------------------------------------------------------------------------- #
#  CLI                                                                         #
# --------------------------------------------------------------------------- #


def banner(msg: str) -> None:
    print(f"\n=== {msg} ===", flush=True)


def die(msg: str, code: int = 2) -> None:
    print(f"FATAL: {msg}", file=sys.stderr, flush=True)
    sys.exit(code)


def parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(
        description="Параллельная сборка клиент+сервер «Чёрная Материя»")
    ap.add_argument("--config", type=Path, default=DEFAULT_CONFIG_PATH)
    ap.add_argument("--targets", default="all",
                    help="all | client | server_linux | server_windows | client,server_linux ...")
    ap.add_argument("--clean", action="store_true", help="полная пересборка")
    ap.add_argument("--no-fail-fast", action="store_true",
                    help="дождаться всех даже при падении одного")
    ap.add_argument("--skip-package", action="store_true")
    ap.add_argument("--dry-run", action="store_true", help="показать команды без запуска")
    return ap.parse_args()


async def main_async() -> int:
    args = parse_args()
    cfg = load_config(args.config)
    pipeline = cfg["pipeline"]
    log_dir = Path(pipeline.get("log_dir", "build/logs"))
    timeout = float(pipeline.get("task_timeout_seconds", 5400))

    wanted = list(TASK_BUILDERS.keys()) if args.targets == "all" \
        else [t.strip() for t in args.targets.split(",")]
    unknown = set(wanted) - set(TASK_BUILDERS)
    if unknown:
        die(f"неизвестные цели: {unknown}. Доступно: {list(TASK_BUILDERS)}")

    tasks = [TASK_BUILDERS[name](cfg, args.clean) for name in wanted]

    banner(f"Цели: {', '.join(t.name for t in tasks)} | fail_fast={not args.no_fail_fast}")
    if args.dry_run:
        for t in tasks:
            print(f"[{t.name}] cwd={t.cwd}\n    $ {' '.join(t.command)}")
        return 0

    t0 = time.monotonic()
    try:
        await run_pipeline(tasks, log_dir, timeout, fail_fast=not args.no_fail_fast)
    except TaskFailure as exc:
        # Fail Fast: выводим хвосты логов ВСЕХ задач (упавшей и отменённых).
        banner("СБОРКА УПАЛА — логи всех задач:")
        for t in tasks:
            print(f"\n----- {t.name}: {t.status} -----")
            print(_tail_file(t.log_path))
        print(f"\nПричина: {exc}")
        return 1
    except Exception as exc:  # noqa: BLE001
        banner(f"Непредвиденная ошибка оркестратора: {exc!r}")
        for t in tasks:
            print(f"\n----- {t.name}: {t.status} -----")
            print(_tail_file(t.log_path))
        return 1

    wall = time.monotonic() - t0
    banner(f"Все сборки успешны за {wall:.1f}s (параллельно)")
    for t in tasks:
        print(f"  {t.name:<15} {t.status:<10} {t.duration:>8.1f}s  log={t.log_path}")

    if not args.skip_package:
        package_artifacts(cfg, tasks)
    return 0


def main() -> None:
    try:
        sys.exit(asyncio.run(main_async()))
    except KeyboardInterrupt:
        print("\nПрервано пользователем", file=sys.stderr)
        sys.exit(130)


if __name__ == "__main__":
    main()
