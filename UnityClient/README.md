# Unity Client — «Чёрная Материя» MMORPG

Базовая структура для миграции с Unreal Engine 5.8 на Unity.

## Установка Unity
1. Установить Unity Hub (https://unity.com/download).
2. Установить редактор Unity 2022.3 LTS (или 6000.x) с модулями:
   - Windows Build Support (IL2CPP)
   - Android Build Support (ARM64)
   - Linux Build Support (optional)

## Структура
- `Assets/Scripts/Network/` — сетевой стек (TCP, фрейминг, SRP-auth, opcodes).
- `Assets/Scripts/UI/` — UI-виджеты (инвентарь, квест-журнал, карта).

## Протокол
Опкоды синхронизированы с `proto/opcodes.defs` (80 опкодов).  
При изменении defs обновлять `EMMOOpcode.cs` через `tools/proto-gen` (TODO).

## Статус миграции
- [ ] Сетевой стек (TCP/KCP, фрейминг, SRP-auth)
- [ ] UI-виджеты (инвентарь, квест-журнал, карта)
- [ ] Удаление UE-клиента
