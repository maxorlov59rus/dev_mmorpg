// UdpListener.h — базовый UDP-листенер
#pragma once
#include <cstdint>

namespace dm {
namespace net {

class UdpListener {
public:
    UdpListener(uint16_t port);
    ~UdpListener();
    bool Start();
    void Stop();
    int Receive(char *buf, int len);
    void Send(const char *buf, int len, const char *host, uint16_t port);
private:
    uint16_t m_port{0};
    int m_sock{-1};
    bool m_running{false};
};

} // namespace net
} // namespace dm
