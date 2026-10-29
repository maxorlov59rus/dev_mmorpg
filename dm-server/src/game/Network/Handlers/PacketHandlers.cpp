#include "game/Network/Handlers/PacketHandlers.h"
#include "game/Network/PacketRouter.h"
#include <iostream>

namespace dm {
namespace net {

void PacketHandlers::RegisterAll() {
    PacketRouter::Instance().Register(
        0x0001, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_AUTH_CHALLENGE received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0003, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_AUTH_PROOF received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0010, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_WORLD_HELLO received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0012, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CHAR_CREATE received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0014, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CHAR_SELECT received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0020, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CHAT_SEND received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0071, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CORRUPTION_QUERY received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x1001, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_MOVE_TICK received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x1012, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_AOI_ACK received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x1013, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_SET_INTEREST received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x1020, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CAST received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x10FF, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_PING received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2000, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_QUEST_QUERY received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2002, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_QUEST_ACCEPT received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2003, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_QUEST_COMPLETE received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2010, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_GOSSIP_HELLO received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2012, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_GOSSIP_SELECT received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x0090, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CLASS_INFO_QUERY received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2020, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_JOIN_LFG received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2024, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_CHOOSE_SPEC received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2027, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_LFG_LEAVE received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2028, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_LFG_PROPOSAL_ACCEPT received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2032, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_TALENT_QUERY received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2034, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_TALENT_LEARN received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2036, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_TALENT_RESET received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2040, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_PVP_TOGGLE received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2043, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_DUEL_REQUEST received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2045, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_DUEL_ACCEPT received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x204A, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_ZONE_MAP_REQUEST received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2050, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_VENDOR_LIST_REQUEST received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2052, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_BUY_ITEM received" << std::endl;
        });
    PacketRouter::Instance().Register(
        0x2054, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
            std::cout << "CMSG_SELL_ITEM received" << std::endl;
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
            std::cout << "CMSG_TRADE_CONFIRM received" << std::endl;
        });
}

} // namespace net
} // namespace dm