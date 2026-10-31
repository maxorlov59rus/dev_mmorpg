#include "game/Network/Handlers/PacketHandlers.h"
#include "game/Network/PacketRouter.h"
#include "common/Logger.h"

namespace dm {
namespace net {

void PacketHandlers::RegisterAll() {
    auto& log = dm::common::Logger::Instance();

    PacketRouter::Instance().Register(
        0x0001, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_AUTH_CHALLENGE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x0003, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_AUTH_PROOF received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x0010, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_WORLD_HELLO received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x0012, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHAR_CREATE received from session " + std::to_string(sessionId));
            // Parse payload: templateIndex(u8), gender(u8), name(str24 null-terminated)
            // TODO: Validate template exists, check name uniqueness in DbPool
            // TODO: Create character entry in database with gender field
            // Send SMSG_CHAR_CREATE_SUCCESS (0x0013) on success
            // Send SMSG_CHAR_CREATE_ERROR (0x0017) with error code on failure:
            //   0 = NameTaken, 1 = InvalidName, 2 = ServerFull, 3 = CharLimitReached
        });
    PacketRouter::Instance().Register(
        0x0014, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHAR_SELECT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x0020, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHAT_SEND received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x0071, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CORRUPTION_QUERY received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x1001, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_MOVE_TICK received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x1012, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_AOI_ACK received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x1013, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SET_INTEREST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x1020, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CAST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x10FF, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_PING received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2000, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_QUEST_QUERY received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2002, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_QUEST_ACCEPT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2003, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_QUEST_COMPLETE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2010, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_GOSSIP_HELLO received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2012, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_GOSSIP_SELECT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x0090, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CLASS_INFO_QUERY received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2020, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_JOIN_LFG received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2024, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHOOSE_SPEC received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2027, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_LFG_LEAVE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2028, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_LFG_PROPOSAL_ACCEPT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2032, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_TALENT_QUERY received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2034, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_TALENT_LEARN received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2036, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_TALENT_RESET received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2040, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_PVP_TOGGLE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2043, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_REQUEST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2045, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_ACCEPT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x204A, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_ZONE_MAP_REQUEST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2050, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_VENDOR_LIST_REQUEST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2052, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_BUY_ITEM received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2054, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SELL_ITEM received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x2056, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_TRADE_REQUEST received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2058, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_TRADE_ACCEPT received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2063, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_PVP_EVENT_JOIN received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x205A, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_TRADE_CONFIRM received from session " + std::to_string(sessionId));
        });

    // Time System handlers
    PacketRouter::Instance().Register(
        0x2052, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_TIME_QUERY received from session " + std::to_string(sessionId));
            // TODO: Send SMSG_TIME_SYNC (0x2050) with current game time + weather
            // Format: hour(u8), minute(u8), day(u16), month(u8), year(u16), weatherType(u8), weatherSeverity(u8)
        });

    // Resurrection System handlers
    PacketRouter::Instance().Register(
        0x1052, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RESURRECT_REQUEST received from session " + std::to_string(sessionId));
            // TODO: Validate player is dead, check resurrect window
            // Send SMSG_RESURRECT (0x1051) with resurrect params
        });
    PacketRouter::Instance().Register(
        0x1053, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RELEASE_SPIRIT received from session " + std::to_string(sessionId));
            // TODO: Transition player to ghost state
            // Send SMSG_DEATH (0x1050) with ghost mode flag
        });
    PacketRouter::Instance().Register(
        0x1054, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RETURN_TO_BODY received from session " + std::to_string(sessionId));
            // TODO: Check player is near corpse, resurrect at corpse
            // Send SMSG_RESURRECT (0x1051) with type=4 (corpse run)
        });

    // Crafting System handlers
    PacketRouter::Instance().Register(
        0x20A0, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CRAFT received from session " + std::to_string(sessionId));
            // Parse payload: playerGuid(u32), recipeId(u32)
            // TODO: Validate player, check profession, materials, skill level
            // Call CraftMgr::Instance().Craft(playerGuid, recipeId)
            // Send SMSG_CRAFT_RESULT (0x20A1) with result code
        });
    PacketRouter::Instance().Register(
        0x20A2, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_LEARN_RECIPE received from session " + std::to_string(sessionId));
            // Parse payload: playerGuid(u32), recipeId(u32)
            // TODO: Validate player has required profession and skill level
            // Call CraftMgr::Instance().LearnRecipe(playerGuid, recipeId)
            // Send SMSG_LEARN_RECIPE_RESULT (0x20A3) with result code
        });
    PacketRouter::Instance().Register(
        0x20A4, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_LEARN_PROFESSION received from session " + std::to_string(sessionId));
            // Parse payload: playerGuid(u32), professionId(u8), playerLevel(u8)
            // TODO: Validate player level >= 20, check max professions (2)
            // Call CraftMgr::Instance().LearnProfession(playerGuid, professionId)
            // Send SMSG_LEARN_PROFESSION_RESULT (0x20A5) with result code
        });
    // Relationship System handlers
    PacketRouter::Instance().Register(
        0x20B0, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FRIEND_REQUEST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20B2, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FRIEND_ACCEPT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20B4, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FRIEND_REMOVE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20B6, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FRIEND_BLOCK received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20B8, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RELATIONSHIP_REQUEST received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20BA, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RELATIONSHIP_ACCEPT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20BC, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RELATIONSHIP_DECLINE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20BE, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_RELATIONSHIP_DISSOLVE received from session " + std::to_string(sessionId));
        });

    PacketRouter::Instance().Register(
        0x3000, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            // Parse GM command from payload
            std::string cmd(payload.begin(), payload.end());
            dm::common::Logger::Instance().Audit("GM command from session " + std::to_string(sessionId) + ": " + cmd);
            dm::common::Logger::Instance().Info("GM command executed: " + cmd);
        });
    // Spirit Pet System handlers
    PacketRouter::Instance().Register(
        0x20C0, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SPIRIT_ROLL received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20C2, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SPIRIT_SET_ACTIVE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20C4, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SPIRIT_REMOVE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20C6, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SPIRIT_ADD_EXP received from session " + std::to_string(sessionId));
        });

    // Duel System handlers
    PacketRouter::Instance().Register(
        0x20D0, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_JOIN received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20D2, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_LEAVE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20D4, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_START received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20D6, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_RESULT received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20D8, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_RATING_UPDATE received from session " + std::to_string(sessionId));
        });
    PacketRouter::Instance().Register(
        0x20DA, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_DUEL_REQUEST_CHALLENGE received from session " + std::to_string(sessionId));
        });

// Achievement System handlers
PacketRouter::Instance().Register(
        0x20E0, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_ACHIEVEMENT_PROGRESS received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x20E2, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_ACHIEVEMENT_COMPLETE received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x20E4, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_ACHIEVEMENT_CLAIM_REWARD received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x20E6, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FACTION_WAR_STATUS received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x20E8, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FACTION_CONTRIBUTE received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x20EA, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_SHOP_BUY received from session " + std::to_string(sessionId));
        });

// Chat System handlers
PacketRouter::Instance().Register(
        0x2100, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHAT_JOIN received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x2102, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHAT_LEAVE received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x2104, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_CHAT_SEND received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x2106, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_WHISPER_SEND received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x2108, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_FRIENDS_LIST received from session " + std::to_string(sessionId));
        });
PacketRouter::Instance().Register(
        0x210A, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            dm::common::Logger::Instance().Debug("CMSG_IGNORE_LIST received from session " + std::to_string(sessionId));
        });
    }

} // namespace net
} // namespace dm
