#include "game/Network/Handlers/PacketHandlers.h"
#include "game/Network/PacketRouter.h"
#include <iostream>

namespace dm {
namespace net {

void PacketHandlers::RegisterAll() {
    // Регистрация обработчиков для CMSG_*
    PacketRouter::Instance().Register(0x0001, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
        std::cout << "CMSG_AUTH_CHALLENGE received" << std::endl;
    });
    PacketRouter::Instance().Register(0x0003, [](uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
        std::cout << "CMSG_AUTH_PROOF received" << std::endl;
    });
    // TODO: добавить остальные обработчики
}

} // namespace net
} // namespace dm
