// KcpClient.cs — заглушка KCP для Unity-клиента
using System;

namespace DarkMatter.Network
{
    public class KcpClient
    {
        public void Connect(string host, int port) { /* TODO */ }
        public void Send(byte[] data) { /* TODO */ }
        public byte[] Receive() { return null; }
        public void Update(uint current) { /* TODO */ }
        public void Close() { /* TODO */ }
    }
}
