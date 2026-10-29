#include "common/network/TcpListener.h"
#include <iostream>

namespace dm {
namespace net {

TcpListener::TcpListener(uint16_t port) : m_port(port) {}

TcpListener::~TcpListener() {
    Stop();
}

bool TcpListener::Start() {
    // TODO: socket/bind/listen
    std::cout << "TcpListener started on port " << m_port << std::endl;
    m_running = true;
    return true;
}

void TcpListener::Stop() {
    m_running = false;
}

int TcpListener::Accept() {
    // TODO: accept
    return -1;
}

} // namespace net
} // namespace dm
