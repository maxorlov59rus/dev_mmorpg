// TcpListener.h — базовый TCP-листенер для сервера
#pragma once
#include <cstdint>
#include <string>

namespace dm {
namespace net {

class TcpListener {
public:
    TcpListener(uint16_t port);
    ~TcpListener();
    bool Start();
    void Stop();
    int Accept(); // returns socket fd or -1
private:
    uint16_t m_port{0};
    int m_listenFd{-1};
    bool m_running{false};
};

} // namespace net
} // namespace dm
