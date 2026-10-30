-- Chat Manager Implementation
-- This file will be created by the build system

#include "game/Chat/ChatMgr.h"

ChatMgr::ChatMgr() : m_chatEnabled(true), m_whisperCooldown(30) {}

ChatMgr::~ChatMgr() {}

void ChatMgr::Initialize() {
    LoadChannels();
    Debug("Chat system initialized");
}

void ChatMgr::Shutdown() {
    // Save all data
    Debug("Chat system shutdown");
}

bool ChatMgr::CreateChannel(uint64 playerGuid, const std::string& name, const std::string& type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    uint64 newId = 1; // Would use DB insert ID
    ChatChannel channel;
    channel.id = newId;
    channel.name = name;
    channel.type = type;
    channel.creatorGuid = playerGuid;
    channel.createdAt = (uint32)time(nullptr);
    channel.isActive = true;
    
    m_channels[newId] = channel;
    
    // Add creator as participant
    channel.participants.push_back(playerGuid);
    m_channels[newId] = channel;
    
    // Save to database
    // SaveChannelToDB(newId, name, type, playerGuid, (uint32)time(nullptr));
    
    Debug("Channel created: " + name + " (" + type + ")");
    return true;
}

bool ChatMgr::JoinChannel(uint64 playerGuid, uint64 channelId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) {
        Debug("Channel not found: " + std::to_string(channelId));
        return false;
    }
    
    ChatChannel& channel = it->second;
    
    // Check level requirement
    // if (channel.levelRequirement > playerLevel) return false;
    
    // Check max participants
    // if (channel.maxParticipants > 0 && (int)channel.participants.size() >= channel.maxParticipants) {
    //     Debug("Channel full: " + channel.name);
    //     return false;
    // }
    
    // Check if already in channel
    for (uint64 pid : channel.participants) {
        if (pid == playerGuid) return true;
    }
    
    channel.participants.push_back(playerGuid);
    m_channels[channelId] = channel;
    
    Debug("Player joined channel: " + channel.name);
    return true;
}

bool ChatMgr::LeaveChannel(uint64 playerGuid, uint64 channelId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_channels.find(channelId);
    if (it == m_channels.end()) return false;
    
    ChatChannel& channel = it->second;
    
    auto pidIt = std::find(channel.participants.begin(), channel.participants.end(), playerGuid);
    if (pidIt != channel.participants.end()) {
        channel.participants.erase(pidIt);
        m_channels[channelId] = channel;
    }
    
    Debug("Player left channel: " + channel.name);
    return true;
}

std::vector<ChatChannel> ChatMgr::GetAvailableChannels(uint64 playerGuid) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ChatChannel> available;
    for (const auto& pair : m_channels) {
        if (pair.second.isActive) {
            available.push_back(pair.second);
        }
    }
    return available;
}

void ChatMgr::SendMessage(uint64 playerGuid, const std::string& text, const std::string& type, uint64 targetGuid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check ignore list
    if (targetGuid > 0 && IsIgnored(playerGuid, targetGuid)) {
        Debug("Player is ignored, message not sent");
        return;
    }
    
    ChatMessage msg;
    msg.id = 1; // Would use DB insert ID
    msg.text = text;
    msg.timestamp = (uint32)time(nullptr);
    msg.isWhisper = (type == "whisper");
    msg.targetPlayerGuid = targetGuid;
    msg.isSystem = false;
    msg.playerGuid = playerGuid;
    
    // Determine channel ID based on type
    // For now, just log and save
    SaveMessage(0, playerGuid, text, msg.isWhisper, targetGuid, false);
    
    // Broadcast to channel participants or whisper target
    // This would involve finding the channel and sending to other players
    Debug("Message sent: " + text.substr(0, 50) + (msg.isWhisper ? " (whisper)" : ""));
}

std::vector<ChatMessage> ChatMgr::GetMessageHistory(uint64 channelId, uint32 limit) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ChatMessage> history;
    // Would query database for messages
    // For now return empty
    return history;
}

std::vector<ChatMessage> ChatMgr::GetWhisperHistory(uint64 playerGuid, uint64 targetGuid, uint32 limit) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ChatMessage> history;
    // Would query database for whisper messages
    return history;
}

void ChatMgr::SetPlayerMuted(uint64 playerGuid, uint64 channelId, bool muted) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_playerMuted[{playerGuid, channelId}] = muted;
}

bool ChatMgr::IsPlayerMuted(uint64 playerGuid, uint64 channelId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_playerMuted.find({playerGuid, channelId});
    if (it != m_playerMuted.end()) return it->second;
    return false;
}

void ChatMgr::AddIgnore(uint64 playerGuid, uint64 ignoredGuid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_ignoreList[{playerGuid, ignoredGuid}] = true;
}

bool ChatMgr::IsIgnored(uint64 playerGuid, uint64 targetGuid) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_ignoreList.find({playerGuid, targetGuid});
    if (it != m_ignoreList.end()) return it->second;
    return false;
}

bool ChatMgr::IsChatEnabled() const {
    return m_chatEnabled;
}

uint32 ChatMgr::GetMuteCooldown() const {
    return m_whisperCooldown;
}