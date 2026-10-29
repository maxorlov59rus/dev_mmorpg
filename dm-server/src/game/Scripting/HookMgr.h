// HookMgr.h — заглушка
#pragma once

namespace dm {
class HookMgr {
public:
    static HookMgr &Instance();
    void CallVoid(const char *name);
};
} // namespace dm
