#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""db_setup.py — автоматическое создание и обновление БД «Чёрной Материи».

Создаёт базы dm_auth, dm_characters, dm_world из sql/create/*.sql,
применяет миграции из sql/upgrades/*.sql,
создаёт файлы обновлений с датой в sql/updates/{auth,characters,world}/.
"""
import argparse
import os
import subprocess
import sys
from datetime import datetime
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
SQL_CREATE = REPO / "sql" / "create"
SQL_UPGRADES = REPO / "sql" / "upgrades"
SQL_UPDATES = REPO / "sql" / "updates"

DBS = {
    "auth": "dm_auth",
    "characters": "dm_characters",
    "world": "dm_world",
}

def run_sql(file_path, db=None):
    """Выполнить SQL-файл через mysql."""
    cmd = ["mysql"]
    if db:
        cmd.append(db)
    cmd.append("-e")
    cmd.append(f"source {file_path}")
    # Actually use mysql < file
    with open(file_path, "r", encoding="utf-8") as f:
        subprocess.run(["mysql", db] if db else ["mysql"], stdin=f, check=True)

def create_dbs():
    """Создать базы данных."""
    for name, db in DBS.items():
        subprocess.run(["mysql", "-e", f"CREATE DATABASE IF NOT EXISTS {db};"], check=True)
        print(f"Created database: {db}")

def apply_create():
    """Применить DDL из sql/create."""
    for sql_file in sorted(SQL_CREATE.glob("*.sql")):
        print(f"Applying {sql_file.name}...")
        # Determine which DB
        if "auth" in sql_file.name:
            run_sql(sql_file, "dm_auth")
        elif "character" in sql_file.name:
            run_sql(sql_file, "dm_characters")
        elif "world" in sql_file.name or "map" in sql_file.name:
            run_sql(sql_file, "dm_world")
        else:
            run_sql(sql_file)

def apply_upgrades():
    """Применить миграции из sql/upgrades."""
    for sql_file in sorted(SQL_UPGRADES.glob("*.sql")):
        print(f"Applying upgrade {sql_file.name}...")
        # Determine DB by content or name
        if "auth" in sql_file.name.lower():
            run_sql(sql_file, "dm_auth")
        elif "character" in sql_file.name.lower():
            run_sql(sql_file, "dm_characters")
        elif "world" in sql_file.name.lower() or "map" in sql_file.name.lower():
            run_sql(sql_file, "dm_world")
        else:
            run_sql(sql_file)

def create_update_file(db_type, description):
    """Создать файл обновления с датой."""
    update_dir = SQL_UPDATES / db_type
    update_dir.mkdir(parents=True, exist_ok=True)
    date_str = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"update_{date_str}_{description}.sql"
    filepath = update_dir / filename
    filepath.write_text("-- Update file\n", encoding="utf-8")
    print(f"Created update file: {filepath}")
    return filepath

def main():
    parser = argparse.ArgumentParser(description="DB setup for Black Matter")
    parser.add_argument("--create", action="store_true", help="Create databases")
    parser.add_argument("--apply-create", action="store_true", help="Apply DDL")
    parser.add_argument("--apply-upgrades", action="store_true", help="Apply upgrades")
    parser.add_argument("--create-update", nargs=2, metavar=("DB_TYPE", "DESCRIPTION"), help="Create update file")
    args = parser.parse_args()

    if args.create:
        create_dbs()
    if args.apply_create:
        apply_create()
    if args.apply_upgrades:
        apply_upgrades()
    if args.create_update:
        db_type, desc = args.create_update
        create_update_file(db_type, desc)

if __name__ == "__main__":
    main()
