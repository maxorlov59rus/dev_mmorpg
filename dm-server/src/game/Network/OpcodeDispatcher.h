// OpcodeDispatcher.h — заглушка
#pragma once

namespace dm {
namespace Network {
class OpcodeDispatcher {
public:
    void Register(int opcode, void *handler) {}
};
} // namespace Network
} // namespace dm
