#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <queue>
#include "DatabaseEnv.h"
#include "Player.h"

struct ChatMessage {
    uint64 id;
    uint64 channelId;
    uint64 playerGuid;
    std::string text;
    uint32 timestamp;
    bool isWhisper;
    uint64 targetPlayerGuid; // For whispers
    bool isSystem;
};

struct ChatChannel {
    uint64 id;
    std::string name;
    std::string type; // global, region, faction, party, whisper, instance, guild
    uint64 creatorGuid;
    uint32 createdAt;
    std::string topic;
    bool isActive;
    std::vector<uint64> participants;
};

class ChatMgr {
public:
    ChatMgr();
    ~ChatMgr();

    void Initialize();
    void Shutdown();

    // Channel management
    bool CreateChannel(uint64 playerGuid, const std::string& name, const std::string& type);
    bool JoinChannel(uint64 playerGuid, uint64 channelId);
    bool LeaveChannel(uint64 playerGuid, uint64 channelId);
    std::vector<ChatChannel> GetAvailableChannels(uint64 playerGuid) const;
    
    // Messaging
    void SendMessage(uint64 playerGuid, const std::string& text, const std::string& type, uint64 targetGuid = 0);
    std::vector<ChatMessage> GetMessageHistory(uint64 channelId, uint32 limit = 50) const;
    std::vector<ChatMessage> GetWhisperHistory(uint64 playerGuid, uint64 targetGuid, uint32 limit = 50) const;
    
    // Player settings
    void SetPlayerMuted(uint64 playerGuid, uint64 channelId, bool muted);
    bool IsPlayerMuted(uint64 playerGuid, uint64 channelId) const;
    void AddIgnore(uint64 playerGuid, uint64 ignoredGuid);
    bool IsIgnored(uint64 playerGuid, uint64 targetGuid) const;
    
    // Configuration
    bool IsChatEnabled() const;
    uint32 GetMuteCooldown() const;

private:
    std::map<uint64, ChatChannel> m_channels;
    std::map<std::pair<uint64, uint64>, bool> m_playerMuted; // player, channel
    std::map<std::pair<uint64, uint64>, bool> m_ignoreList; // player, ignored
    std::mutex m_mutex;
    bool m_chatEnabled;
    uint32 m_whisperCooldown;

    void LoadChannels();
    void LoadPlayerSettings(uint64 playerGuid);
    void SaveMessage(uint64 channelId, uint64 playerGuid, const std::string& text, 
                     bool isWhisper, uint64 targetGuid, bool isSystem);
};