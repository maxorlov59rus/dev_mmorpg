// PacketFraming.h — фрейминг пакетов (4-байтный заголовок + payload)
#pragma once
#include <cstdint>
#include <vector>

namespace dm {
namespace net {

class PacketFraming {
public:
    static std::vector<uint8_t> Frame(uint16_t opcode, const std::vector<uint8_t> &payload);
    static bool TryUnframe(const std::vector<uint8_t> &data, uint16_t &opcode, std::vector<uint8_t> &payload, size_t &consumed);
};

} // namespace net
} // namespace dm
