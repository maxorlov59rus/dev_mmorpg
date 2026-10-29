#include "common/network/PacketFraming.h"
#include <cstring>

namespace dm {
namespace net {

std::vector<uint8_t> PacketFraming::Frame(uint16_t opcode, const std::vector<uint8_t> &payload) {
    std::vector<uint8_t> frame;
    uint32_t size = 4 + payload.size();
    frame.push_back(size & 0xFF);
    frame.push_back((size >> 8) & 0xFF);
    frame.push_back(opcode & 0xFF);
    frame.push_back((opcode >> 8) & 0xFF);
    frame.insert(frame.end(), payload.begin(), payload.end());
    return frame;
}

bool PacketFraming::TryUnframe(const std::vector<uint8_t> &data, uint16_t &opcode,
                               std::vector<uint8_t> &payload, size_t &consumed) {
    if (data.size() < 4)
        return false;
    uint32_t size = data[0] | (data[1] << 8);
    if (size < 4 || size > 512 * 1024)
        return false;
    if (data.size() < size)
        return false;
    opcode = data[2] | (data[3] << 8);
    payload.assign(data.begin() + 4, data.begin() + size);
    consumed = size;
    return true;
}

} // namespace net
} // namespace dm
