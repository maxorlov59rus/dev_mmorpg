#include "game/Quests/QuestEngine.h"
#include "common/Logger.h"
#include <string>

namespace dm {

QuestEngine &QuestEngine::Instance() {
    static QuestEngine inst;
    return inst;
}

void QuestEngine::LoadScript(const std::string &path) {
    auto& log = dm::common::Logger::Instance();
    log.Info("Loading quest script: " + path);
    if (!L) L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        log.Error("Failed to load quest script: " + path + " - " + lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        log.Info("Quest script loaded: " + path);
    }
}

void QuestEngine::OnAccept(uint32_t questId, uint64_t playerGuid) {
    auto& log = dm::common::Logger::Instance();
    if (!L) return;
    lua_getglobal(L, "OnQuestAccept");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, questId);
        lua_pushinteger(L, playerGuid);
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            log.Error("Quest OnAccept script error: " + std::string(lua_tostring(L, -1)));
            lua_pop(L, 1);
        } else {
            log.Debug("Quest OnAccept executed: questId=" + std::to_string(questId) + ", playerGuid=" + std::to_string(playerGuid));
        }
    } else {
        lua_pop(L, 1);
    }
}

void QuestEngine::OnComplete(uint32_t questId, uint64_t playerGuid) {
    auto& log = dm::common::Logger::Instance();
    if (!L) return;
    lua_getglobal(L, "OnQuestComplete");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, questId);
        lua_pushinteger(L, playerGuid);
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            log.Error("Quest OnComplete script error: " + std::string(lua_tostring(L, -1)));
            lua_pop(L, 1);
        } else {
            log.Debug("Quest OnComplete executed: questId=" + std::to_string(questId) + ", playerGuid=" + std::to_string(playerGuid));
        }
    } else {
        lua_pop(L, 1);
    }
}

void QuestEngine::OnProgress(uint32_t questId, uint64_t playerGuid, uint32_t objectiveId, uint32_t count) {
    auto& log = dm::common::Logger::Instance();
    if (!L) return;
    lua_getglobal(L, "OnQuestProgress");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, questId);
        lua_pushinteger(L, playerGuid);
        lua_pushinteger(L, objectiveId);
        lua_pushinteger(L, count);
        if (lua_pcall(L, 4, 0, 0) != LUA_OK) {
            log.Error("Quest OnProgress script error: " + std::string(lua_tostring(L, -1)));
            lua_pop(L, 1);
        } else {
            log.Debug("Quest OnProgress executed: questId=" + std::to_string(questId) + ", playerGuid=" + std::to_string(playerGuid) + ", objectiveId=" + std::to_string(objectiveId) + ", count=" + std::to_string(count));
        }
    } else {
        lua_pop(L, 1);
    }
}

void QuestEngine::OnFail(uint32_t questId, uint64_t playerGuid) {
    auto& log = dm::common::Logger::Instance();
    if (!L) return;
    lua_getglobal(L, "OnQuestFail");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, questId);
        lua_pushinteger(L, playerGuid);
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            log.Error("Quest OnFail script error: " + std::string(lua_tostring(L, -1)));
            lua_pop(L, 1);
        } else {
            log.Debug("Quest OnFail executed: questId=" + std::to_string(questId) + ", playerGuid=" + std::to_string(playerGuid));
        }
    } else {
        lua_pop(L, 1);
    }
}

void QuestEngine::OnAbandon(uint32_t questId, uint64_t playerGuid) {
    auto& log = dm::common::Logger::Instance();
    if (!L) return;
    lua_getglobal(L, "OnQuestAbandon");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, questId);
        lua_pushinteger(L, playerGuid);
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            log.Error("Quest OnAbandon script error: " + std::string(lua_tostring(L, -1)));
            lua_pop(L, 1);
        } else {
            log.Debug("Quest OnAbandon executed: questId=" + std::to_string(questId) + ", playerGuid=" + std::to_string(playerGuid));
        }
    } else {
        lua_pop(L, 1);
    }
}

void QuestEngine::OnObjectiveUpdate(uint32_t questId, uint64_t playerGuid, uint32_t objectiveId, uint32_t count) {
    auto& log = dm::common::Logger::Instance();
    if (!L) return;
    lua_getglobal(L, "OnObjectiveUpdate");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, questId);
        lua_pushinteger(L, playerGuid);
        lua_pushinteger(L, objectiveId);
        lua_pushinteger(L, count);
        if (lua_pcall(L, 4, 0, 0) != LUA_OK) {
            log.Error("Quest OnObjectiveUpdate script error: " + std::string(lua_tostring(L, -1)));
            lua_pop(L, 1);
        } else {
            log.Debug("Quest OnObjectiveUpdate executed: questId=" + std::to_string(questId) + ", playerGuid=" + std::to_string(playerGuid) + ", objectiveId=" + std::to_string(objectiveId) + ", count=" + std::to_string(count));
        }
    } else {
        lua_pop(L, 1);
    }
}

void QuestEngine::RegisterHook(const std::string &hookName, const std::string &luaFunc) {
    if (!L) return;
    lua_getglobal(L, luaFunc.c_str());
    if (lua_isfunction(L, -1)) {
        lua_setglobal(L, hookName.c_str());
    } else {
        lua_pop(L, 1);
    }
}

} // namespace dm
