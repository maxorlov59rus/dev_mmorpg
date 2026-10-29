// PacketRouter.h — диспетчер пакетов
#pragma once
#include <cstdint>
#include <functional>
#include <map>

namespace dm {
namespace net {

class PacketRouter {
public:
    using Handler = std::function<void(uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload)>;
    static PacketRouter &Instance();
    void Register(uint16_t opcode, Handler handler);
    void Route(uint32_t sessionId, uint16_t opcode, const std::vector<uint8_t> &payload);
private:
    std::map<uint16_t, Handler> m_handlers;
};

} // namespace net
} // namespace dm
