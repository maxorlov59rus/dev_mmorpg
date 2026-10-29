#include "common/network/TcpListener.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

namespace dm {
namespace net {

TcpListener::TcpListener(uint16_t port) : m_port(port) {}

TcpListener::~TcpListener() {
    Stop();
}

bool TcpListener::Start() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd == INVALID_SOCKET) {
        std::cerr << "socket failed" << std::endl;
        WSACleanup();
        return false;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);
    if (bind(m_listenFd, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind failed" << std::endl;
        closesocket(m_listenFd);
        WSACleanup();
        return false;
    }
    if (listen(m_listenFd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed" << std::endl;
        closesocket(m_listenFd);
        WSACleanup();
        return false;
    }
    m_running = true;
    std::cout << "TcpListener started on port " << m_port << std::endl;
    return true;
}

void TcpListener::Stop() {
    m_running = false;
    if (m_listenFd != -1) {
        closesocket(m_listenFd);
        m_listenFd = -1;
    }
    WSACleanup();
}

int TcpListener::Accept() {
    if (!m_running)
        return -1;
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    int clientFd = accept(m_listenFd, (sockaddr *)&clientAddr, &len);
    return clientFd;
}

} // namespace net
} // namespace dm