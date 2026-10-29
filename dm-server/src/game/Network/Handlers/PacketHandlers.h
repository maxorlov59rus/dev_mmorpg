// PacketHandlers.h — обработчики пакетов
#pragma once
#include <cstdint>

namespace dm {
namespace net {

class PacketHandlers {
public:
    static void RegisterAll();
};

} // namespace net
} // namespace dm
