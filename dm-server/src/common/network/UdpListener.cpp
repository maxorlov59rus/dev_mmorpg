#include "common/network/UdpListener.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

namespace dm {
namespace net {

UdpListener::UdpListener(uint16_t port) : m_port(port) {}

UdpListener::~UdpListener() {
    Stop();
}

bool UdpListener::Start() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }
    m_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sock == INVALID_SOCKET) {
        std::cerr << "socket failed" << std::endl;
        WSACleanup();
        return false;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);
    if (bind(m_sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind failed" << std::endl;
        closesocket(m_sock);
        WSACleanup();
        return false;
    }
    m_running = true;
    std::cout << "UdpListener started on port " << m_port << std::endl;
    return true;
}

void UdpListener::Stop() {
    m_running = false;
    if (m_sock != -1) {
        closesocket(m_sock);
        m_sock = -1;
    }
    WSACleanup();
}

int UdpListener::Receive(char *buf, int len) {
    if (!m_running) return -1;
    sockaddr_in clientAddr{};
    int addrLen = sizeof(clientAddr);
    int received = recvfrom(m_sock, buf, len, 0, (sockaddr*)&clientAddr, &addrLen);
    return received;
}

void UdpListener::Send(const char *buf, int len, const char *host, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    sendto(m_sock, buf, len, 0, (sockaddr*)&addr, sizeof(addr));
}

} // namespace net
} // namespace dm
