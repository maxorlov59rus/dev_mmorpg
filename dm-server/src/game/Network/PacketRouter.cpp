#include "game/Network/PacketRouter.h"
#include <functional>
#include <map>
#include <vector>

namespace dm {
namespace net {

PacketRouter &PacketRouter::Instance() {
    static PacketRouter inst;
    return inst;
}

void PacketRouter::Register(uint16_t opcode, Handler handler) {
    m_handlers[opcode] = handler;
}

void PacketRouter::Route(uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload) {
    auto it = m_handlers.find(opcode);
    if (it != m_handlers.end()) {
        it->second(sessionId, opcode, payload);
    }
}

} // namespace net
} // namespace dm
