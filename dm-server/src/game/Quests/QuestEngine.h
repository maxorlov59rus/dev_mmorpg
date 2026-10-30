// QuestEngine.h — квест-движок (Lua-скриптинг)
#pragma once
#include <cstdint>
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace dm {

class QuestEngine {
public:
    static QuestEngine &Instance();
    void LoadScript(const std::string &path);
    void OnAccept(uint32_t questId, uint64_t playerGuid);
    void OnComplete(uint32_t questId, uint64_t playerGuid);
    void OnProgress(uint32_t questId, uint64_t playerGuid, uint32_t objectiveId, uint32_t count);
    void OnFail(uint32_t questId, uint64_t playerGuid);
    void OnAbandon(uint32_t questId, uint64_t playerGuid);
    void OnObjectiveUpdate(uint32_t questId, uint64_t playerGuid, uint32_t objectiveId, uint32_t count);
    void RegisterHook(const std::string &hookName, const std::string &luaFunc);
    lua_State *GetLuaState() { return L; }

private:
    lua_State *L = nullptr;
};

} // namespace dm
