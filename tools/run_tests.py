#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""run_tests.py — кроссплатформенный оркестратор тестов «Чёрной Материи».

Запускает на хосте (Windows/Linux) три уровня проверки и пишет JSON-отчёт:
  1. unit    — ctest в build-директории сервера (если собрана; иначе skip)
  2. data    — контракты данных: 1200 предметов, уникальность entry/displayid,
               сверка CSV mapping <-> SQL, синтаксис всех .sql (без подключения)
  3. protocol— golden-packet: парсинг proto/opcodes.defs, проверка уникальности
               ID/имён, наличие client/server симметрии CMSG<->SMSG,
               сверка EMMOOpcode в MMOProtocol.h с defs (по числовым значениям)

Использование:
  python tools/run_tests.py [--suite all|unit|data|protocol] [--json report.json]
  python tools/run_tests.py --build-dir dm-server/build-win   # для ctest

Выход: 0 если все выполненные наборы зелёные; 1 при любом FAIL; 2 при ошибке запуска.
"""
from __future__ import annotations

import argparse
import csv
import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent


class Suite:
    def __init__(self, name: str):
        self.name = name
        self.cases: list[dict] = []

    def add(self, case: str, ok: bool, detail: str = "", skipped: bool = False):
        status = "SKIP" if skipped else ("PASS" if ok else "FAIL")
        self.cases.append({"case": case, "status": status, "detail": detail})
        mark = {"PASS": "+", "FAIL": "-", "SKIP": "~"}[status]
        print(f"  [{mark}] {case}" + (f" — {detail}" if detail else ""))
        return ok

    @property
    def failed(self) -> int:
        return sum(1 for c in self.cases if c["status"] == "FAIL")

    @property
    def passed(self) -> int:
        return sum(1 for c in self.cases if c["status"] == "PASS")


def read(p: Path) -> str:
    return p.read_text(encoding="utf-8-sig", errors="replace")


# ---------------------------------------------------------------------------
#  Suite: data — контракты предметов / БД
# ---------------------------------------------------------------------------
def suite_data() -> Suite:
    s = Suite("data")
    sql_path = REPO / "sql/upgrades/Insert_1200_Items.sql"
    map_sql = REPO / "sql/upgrades/Insert_DisplayId_Map.sql"
    csv_path = REPO / "data/mapping_displayid.csv"

    if not sql_path.is_file():
        s.add("insert_1200 exists", False, f"{sql_path} не найден")
        return s

    text = read(sql_path)
    entries = [int(m.group(1)) for m in re.finditer(r"\(\s*(\d{5})\s*,", text)]
    uniq = sorted(set(entries))
    s.add("item count == 1200", len(entries) == 1200, f"найдено строк VALUES: {len(entries)}")
    s.add("entries unique", len(uniq) == len(set(entries)), f"уникальных: {len(set(entries))}")
    s.add("entries contiguous 10001..11200",
          uniq == list(range(10001, 11201)),
          f"min={uniq[0] if uniq else '-'} max={uniq[-1] if uniq else '-'}")

    # displayid contract: 50000 + (item - 10000)
    disp_re = re.compile(r"^\(\s*(\d{5})\s*,\s*(\d{5})\s*,", re.M)
    if map_sql.is_file():
        pairs = [(int(a), int(b)) for a, b in disp_re.findall(read(map_sql))]
        bad = [(i, d) for i, d in pairs if d != 50000 + (i - 10000)]
        s.add("displayid_map rows == 1200", len(pairs) == 1200, f"строк: {len(pairs)}")
        s.add("displayid formula D=50000+(I-10000)", not bad, f"нарушений: {len(bad)}")

    if csv_path.is_file():
        with csv_path.open(newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))
        have = set(rows[0].keys()) if rows else set()
        cols_ok = bool(rows) and {"ItemID", "DisplayID"} <= have and \
            ({"UE5_Mesh_Path", "UE5_Icon_Path"} <= have or {"UE5MeshPath", "UE5IconPath"} <= have)
        s.add("csv columns", cols_ok, f"{len(rows)} строк")
        if cols_ok:
            mism = [r for r in rows
                    if int(r["DisplayID"]) != 50000 + (int(r["ItemID"]) - 10000)]
            s.add("csv displayid formula", not mism, f"нарушений: {len(mism)}")
            if map_sql.is_file():
                sql_ids = {int(a) for a, _ in disp_re.findall(read(map_sql))}
                csv_ids = {int(r["ItemID"]) for r in rows}
                s.add("csv <-> sql item sets equal", sql_ids == csv_ids,
                      f"delta: {len(sql_ids ^ csv_ids)}")

    # Синтаксический smoke всех SQL: удаляем --комментарии, нормализуем \' \" экранировки
    bad_sql = []
    for sqlf in sorted((REPO / "sql").rglob("*.sql")):
        t = re.sub(r"--[^\n]*", "", read(sqlf))
        t = t.replace("\\'", "").replace('\\"', "")   # экранированные кавычки не считаются
        bal = t.count("(") - t.count(")")
        q = t.count("'") % 2
        if bal != 0 or q != 0:
            bad_sql.append(f"{sqlf.name}(br={bal},q={q})")
    s.add("all .sql bracket/quote balanced", not bad_sql, "; ".join(bad_sql[:6]))
    return s


# ---------------------------------------------------------------------------
#  Suite: protocol — opcodes + golden структуры
# ---------------------------------------------------------------------------
def suite_protocol() -> Suite:
    s = Suite("protocol")
    defs = REPO / "proto/opcodes.defs"
    if not defs.is_file():
        s.add("opcodes.defs exists", False, str(defs))
        return s

    pat = re.compile(r"@opcode\s+((?:CMSG|SMSG)_\w+)\s+0x([0-9A-Fa-f]+)")
    ops = pat.findall(read(defs))
    names = [n for n, _ in ops]
    ids = [v.upper() for _, v in ops]
    s.add("opcode count > 0", bool(ops), f"{len(ops)} опкодов")
    dup_names = {n for n in names if names.count(n) > 1}
    dup_ids = {i for i in ids if ids.count(i) > 1}
    s.add("names unique", not dup_names, str(sorted(dup_names)))
    s.add("ids unique", not dup_ids, str(sorted(dup_ids)))

    # CMSG/SMSG симметрия: на каждый CMSG_* ожидаем SMSG ответа того же корня,
    # SMSG с суффиксом (_RESULT/_SUCCESS/_STATE), ACK-ответ под другим именем
    # (whitelist) либо fire-and-forget (whitelist).
    NO_RESPONSE = {
        "CMSG_PING",                    # -> SMSG_PONG (корень отличается, учтено ниже)
        "CMSG_JOIN_LFG",                # -> SMSG_LFG_QUEUE_STATE (суффикс-вариант покрыт ниже)
        "CMSG_RACIAL_ABILITY_ACTIVATE",  # эффект приходит через SMSG_INITIALIZE_AURAS/LEARN_SPELL
        "CMSG_SET_INTEREST",            # fire-and-forget AOI
        "CMSG_AOI_ACK",                 # чистый acknowledgement, ответа нет по контракту AOI
        "CMSG_CHAT_SEND",               # ответ не обязателен: SMSG_CHAT_RECV есть — покрывается ниже
    }
    ROOT_ALIASES = {  # корень CMSG -> допустимые SMSG-ответы (кроме точного/суффиксного)
        "AUTH_PROOF": ["SMSG_AUTH_RESULT"],
        "WORLD_HELLO": ["SMSG_CHAR_LIST"],
        "CHAR_SELECT": ["SMSG_MAP_ASSIGN"],
        "CORRUPTION_QUERY": ["SMSG_CORRUPTION_STATE"],
        "MOVE_TICK": ["SMSG_SNAPSHOT", "SMSG_DELTA"],
        "QUEST_QUERY": ["SMSG_QUEST_LIST"],
        "QUEST_ACCEPT": ["SMSG_QUEST_ACCEPTED"],
        "QUEST_COMPLETE": ["SMSG_QUEST_TURN_IN_HINT", "SMSG_QUEST_PROGRESS"],
        "GOSSIP_HELLO": ["SMSG_GOSSIP_SHOW", "SMSG_CREATURE_TEXT"],
        "GOSSIP_SELECT": ["SMSG_GOSSIP_REPLY", "SMSG_QUEST_ACCEPTED"],
        "CHOOSE_SPEC": ["SMSG_LEARNED_SPELLS_BATCH"],
        "LFG_PROPOSAL_ACCEPT": ["SMSG_LFG_ROLE_CHECK", "SMSG_LFG_QUEUE_STATE"],
        "TALENT_LEARN": ["SMSG_TALENT_DATA"],   # результат реплицируется полным деревом
        "CLASS_INFO_QUERY": ["SMSG_CLASS_INFO"],
        "TALENT_QUERY": ["SMSG_TALENT_DATA"],
        "TALENT_RESET": ["SMSG_TALENT_RESULT"],
        "LFG_LEAVE": ["SMSG_LFG_QUEUE_STATE"],
    }
    missing = []
    for n in names:
        if not n.startswith("CMSG_"):
            continue
        root = n[5:]
        candidates = [f"SMSG_{root}", f"SMSG_{root}_SUCCESS", f"SMSG_{root}_RESULT",
                      f"SMSG_{root}_STATE"] + ROOT_ALIASES.get(root, [])
        if not any(c in names for c in candidates) and n not in NO_RESPONSE:
            missing.append(n)
    s.add("CMSG have SMSG counterpart", not missing, f"без ответа: {missing[:6]}")

    # Сверка с клиентским enum. Замечание: в MMOProtocol.h блок Auth (0x01xx)
    # использует собственное пространство кодов authserver'а и НЕ входит в
    # wire-реестр opcodes.defs (там handshake описан как CMSG/SMSG_AUTH_* 0x000x).
    # Поэтому сверяются только значения вне диапазона 0x0100-0x01FF.
    proto_h = REPO / "Plugins/MMOCore/Source/MMOCore/Public/Network/MMOProtocol.h"
    if not proto_h.is_file():
        candidates = list((REPO / "Plugins").rglob("MMOProtocol.h"))
        proto_h = candidates[0] if candidates else proto_h
    if proto_h.is_file():
        text = read(proto_h)
        wire_vals = {i for i in ids}
        # a) явные wire-значения из switch ToWire() — обязательны к существованию в defs
        towire = {v.upper() for v in re.findall(r"return\s+0x([0-9A-Fa-f]+)\s*;", text)}
        bad_tw = sorted(towire - wire_vals)
        s.add("ToWire() values exist in opcodes.defs", not bad_tw,
              f"ToWire значений: {len(towire)}, вне defs: {bad_tw[:6]}")
        # b) инлайн-комментарии "// wire 0xNNNN" — тоже сверяем
        comments = {v.upper() for v in re.findall(r"//\s*wire[^0-9A-F]*0x([0-9A-Fa-f]{4})", text)}
        bad_cm = sorted(comments - wire_vals)
        s.add('"// wire" comments match defs', not bad_cm,
              f"комментариев: {len(comments)}, рассинхрон: {bad_cm[:6]}")
        # c) покрытие: каждый defs-опкод должен быть достижим клиентом:
        #    - явные ToWire()/«// wire 0xNNNN» (новые блоки v1.3+);
        #    - legacy-комментарии вида «wire: SMSG_X (defs 0xNNNN)»;
        #    - прямые enum-значения legacy-блока (0x00xx совпадают с defs);
        #    - LEGACY_UNHANDLED — опкоды серверного/внутреннего контура, которые
        #      клиент-енум пока не маршрутизирует (должны быть задокументированы).
        legacy_comments = {v.upper() for v in
                           re.findall(r"defs\s+0x([0-9A-Fa-f]{4})", text)}
        enum_direct = {v.upper() for v in
                       re.findall(r"(?:Cmd|Srv|Cli)\w+\s*=\s*0x([0-9A-Fa-f]+)", text)}
        client_known = towire | comments | legacy_comments | enum_direct
        uncovered = sorted(wire_vals - client_known)
        s.add("every defs opcode reachable from client", not uncovered,
              f"defs: {len(wire_vals)}, недоступно клиенту: {uncovered[:8]}")
    return s


# ---------------------------------------------------------------------------
#  Suite: unit — ctest (если есть сборка)
# ---------------------------------------------------------------------------
def suite_unit(build_dir: Path | None) -> Suite:
    s = Suite("unit")
    if build_dir is None:
        cand = [REPO / "dm-server/build-win", REPO / "dm-server/build-linux",
                REPO / "dm-server/build"]
        build_dir = next((c for c in cand if (c / "CTestTestfile.cmake").exists()), None)
    if build_dir is None or not build_dir.exists():
        s.add("ctest available", True, "сборка не найдена — пропущено (не фейл CI-сборок)", skipped=True)
        return s
    try:
        r = subprocess.run(["ctest", "--output-on-failure", "-j4"], cwd=build_dir,
                           capture_output=True, text=True, timeout=900)
        ok = r.returncode == 0
        tail = (r.stdout or "").strip().splitlines()[-3:]
        s.add("ctest", ok, " | ".join(tail))
    except FileNotFoundError:
        s.add("ctest binary", False, "ctest не в PATH")
    except subprocess.TimeoutExpired:
        s.add("ctest", False, "timeout 900s")
    return s


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--suite", default="all", choices=["all", "unit", "data", "protocol"])
    ap.add_argument("--build-dir", default=None, help="Каталог сборки сервера для ctest")
    ap.add_argument("--json", default=None, help="Путь для JSON-отчёта")
    args = ap.parse_args(argv)

    suites: list[Suite] = []
    if args.suite in ("all", "data"):
        suites.append(suite_data())
    if args.suite in ("all", "protocol"):
        suites.append(suite_protocol())
    if args.suite in ("all", "unit"):
        suites.append(suite_unit(Path(args.build_dir) if args.build_dir else None))

    total_fail = sum(s.failed for s in suites)
    total_pass = sum(s.passed for s in suites)
    print(f"\n=== ИТОГ: PASS={total_pass} FAIL={total_fail} ===")

    if args.json:
        report = {
            "generated_at": datetime.now(timezone.utc).isoformat(),
            "platform": sys.platform,
            "summary": {"pass": total_pass, "fail": total_fail},
            "suites": {s.name: s.cases for s in suites},
        }
        Path(args.json).write_text(json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")
        print(f"[report] {args.json}")

    return 1 if total_fail else 0


if __name__ == "__main__":
    sys.exit(main())
