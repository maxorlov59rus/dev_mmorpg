#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""SyncUProject.py — идемпотентная синхронизация DMClient.uproject.

Сканирует:
  * Source/                       (игровые модули проекта)
  * Plugins/<Plugin>/Source/*     (модули плагинов, включая MMOCore)
и мержит недостающие записи в секции "Modules" / "Plugins" файла .uproject.

Правила:
  - Модуль обнаруживается по наличию <Name>.Build.cs в папке; тип (Runtime/Editor)
    определяется по содержимому Build.cs (DefaultBuildType / "Editor").
  - Папки Private/ Public/ Intermediate/ не являются модулями.
  - Если модуль есть в JSON, но папка удалена — запись СОХРАНЯЕТСЯ (безопасно),
    выводится предупреждение.
  - EngineAssociation принудительно ставится в --engine-assoc (по умолчанию 5.8.3).
  - Форматирование JSON сохраняется: indent=2, ключи без сортировки (порядок
    словаря Python = порядок исходного файла для существующих ключей).

CLI:
  python tools/SyncUProject.py [--project PATH] [--dry-run] [--engine-assoc VER]
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

# Плагины движка, необходимые текущим фичам (GAS, UMG/CommonUI, Niagara, OSS).
REQUIRED_PLUGINS = {
    "GameplayAbilities": {"Enabled": True},
    "CommonUI": {"Enabled": True},
    "Niagara": {"Enabled": True},
    "OnlineSubsystem": {"Enabled": True},
    "OnlineSubsystemUtils": {"Enabled": True},
    "WorldPartition": {"Enabled": True},
}

SKIP_DIRS = {"Private", "Public", "Intermediate", "Binaries", "Resources", "Config"}


def detect_module_type(build_cs: Path) -> str:
    """Runtime|Editor по содержимому *.Build.cs."""
    try:
        text = build_cs.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return "Runtime"
    if "DefaultBuildType = BuildTargetsType.Editor" in text or "ModuleType.Editor" in text:
        return "Editor"
    # конвенция имён: <X>Editor* — редакторный модуль
    if build_cs.stem.endswith("Editor"):
        return "Editor"
    return "Runtime"


def scan_modules(repo_root: Path) -> list[dict]:
    """Обход Source/ и Plugins/*/Source/*/ на наличие *.Build.cs."""
    found: dict[str, dict] = {}

    def add(name: str, mtype: str, path: Path) -> None:
        if name in found:
            return
        rel = path.relative_to(repo_root).as_posix()
        entry = {
            "Name": name,
            "Type": mtype,
            "LoadingPhase": "Default" if mtype == "Runtime" else "Default",
        }
        # Для модулей плагинов UE не требует AdditionalDependencies — путь только для лога
        entry["_source"] = rel
        found[name] = entry

    # Корневые игровые модули
    src = repo_root / "Source"
    if src.is_dir():
        for build_cs in sorted(src.glob("*.Build.cs")) + sorted(src.glob("*/*.Build.cs")):
            mod_dir = build_cs.parent
            # имя модуля = имя папки; для плоской структуры Source/X.Build.cs -> X
            name = mod_dir.name if mod_dir != src else build_cs.stem
            add(name, detect_module_type(build_cs), mod_dir)

    # Модули плагинов
    plugins_dir = repo_root / "Plugins"
    if plugins_dir.is_dir():
        for plugin_src in sorted(plugins_dir.glob("*/Source")):
            for mod_dir in sorted(p for p in plugin_src.iterdir() if p.is_dir()):
                if mod_dir.name in SKIP_DIRS:
                    continue
                build_cs = mod_dir / f"{mod_dir.name}.Build.cs"
                if not build_cs.exists():
                    candidates = list(mod_dir.glob("*.Build.cs"))
                    if not candidates:
                        continue
                    build_cs = candidates[0]
                add(mod_dir.name, detect_module_type(build_cs), mod_dir)

    return list(found.values())


def sync(uproject_path: Path, dry_run: bool, engine_assoc: str) -> int:
    if not uproject_path.is_file():
        print(f"[FATAL] не найден {uproject_path}", file=sys.stderr)
        return 2

    data = json.loads(uproject_path.read_text(encoding="utf-8-sig"))
    original_keys = list(data.keys())

    changes: list[str] = []

    # --- EngineAssociation (принудительно) -----------------------------------
    if data.get("EngineAssociation") != engine_assoc:
        changes.append(f"EngineAssociation: {data.get('EngineAssociation')!r} -> {engine_assoc!r}")
        data["EngineAssociation"] = engine_assoc

    # --- Modules --------------------------------------------------------------
    modules = data.setdefault("Modules", [])
    existing_names = {m.get("Name") for m in modules if isinstance(m, dict)}
    scanned = scan_modules(uproject_path.parent)

    for mod in scanned:
        clean = {k: v for k, v in mod.items() if not k.startswith("_")}
        if mod["Name"] not in existing_names:
            modules.append(clean)
            changes.append(f"+ module {mod['Name']} ({mod['Type']}) <- {mod['_source']}")
            existing_names.add(mod["Name"])

    # Потерянные модули — оставляем, предупреждаем
    live_names = {m["Name"] for m in scanned}
    for m in modules:
        name = m.get("Name")
        if name and name not in live_names:
            changes.append(f"! module {name} в JSON, но папка не найдена — сохранён как есть")

    # --- Plugins ---------------------------------------------------------------
    plugins = data.setdefault("Plugins", [])
    existing_plugins = {p.get("Name") for p in plugins if isinstance(p, dict)}
    for pname, cfg in REQUIRED_PLUGINS.items():
        if pname not in existing_plugins:
            plugins.append({"Name": pname, **cfg})
            changes.append(f"+ plugin {pname}")

    # Локальные плагины из Plugins/*.uplugin
    for uplugin in sorted((uproject_path.parent / "Plugins").glob("*/**/*.uplugin")):
        try:
            meta = json.loads(uplugin.read_text(encoding="utf-8"))
            pname = meta.get("Name") or uplugin.stem
        except (json.JSONDecodeError, OSError):
            pname = uplugin.parent.parent.name
        if pname and pname not in {p.get("Name") for p in plugins}:
            plugins.append({"Name": pname, "Enabled": True})
            changes.append(f"+ local plugin {pname} ({uplugin.parent.parent.name})")

    # Сохраняем порядок верхнеуровневых ключей исходного файла
    ordered = {k: data[k] for k in original_keys if k in data}
    for k, v in data.items():
        if k not in ordered:
            ordered[k] = v

    if not changes:
        print("[sync] изменений нет — .uproject актуален (идемпотентность OK)")
        return 0

    print(f"[sync] обнаружено изменений: {len(changes)}")
    for c in changes:
        print(f"  {c}")

    if dry_run:
        print("[sync] DRY-RUN: файл НЕ записан")
        return 0

    text = json.dumps(ordered, indent=2, ensure_ascii=False) + "\n"
    uproject_path.write_text(text, encoding="utf-8")
    print(f"[sync] записано: {uproject_path}")
    return 0


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Синхронизация DMClient.uproject с состоянием репозитория")
    ap.add_argument("--project", default="DMClient.uproject", help="Путь к .uproject")
    ap.add_argument("--dry-run", action="store_true", help="Показать изменения без записи")
    ap.add_argument("--engine-assoc", default="5.8.3", help="Принудительное значение EngineAssociation")
    args = ap.parse_args(argv)

    return sync(Path(args.project).resolve(), args.dry_run, args.engine_assoc)


if __name__ == "__main__":
    sys.exit(main())
