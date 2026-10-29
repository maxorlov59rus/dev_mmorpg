// MMONetworkClient.cs — базовый TCP-клиент (зеркало FMMONetworkClient)
using System;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;

namespace DarkMatter.Network
{
    public class MMONetworkClient
    {
        private TcpClient _client;
        private NetworkStream _stream;
        private Thread _recvThread;
        private bool _running;

        public bool IsConnected => _client != null && _client.Connected;

        public void Connect(string host, int port)
        {
            _client = new TcpClient();
            _client.Connect(host, port);
            _stream = _client.GetStream();
            _running = true;
            _recvThread = new Thread(ReceiveLoop);
            _recvThread.Start();
        }

        private void ReceiveLoop()
        {
            byte[] buffer = new byte[4096];
            List<byte> accumulated = new List<byte>();
            while (_running)
            {
                try
                {
                    int read = _stream.Read(buffer, 0, buffer.Length);
                    if (read <= 0) break;
                    for (int i = 0; i < read; i++) accumulated.Add(buffer[i]);
                    // Parse frames
                    int consumed = 0;
                    while (accumulated.Count >= 4)
                    {
                        byte[] data = accumulated.ToArray();
                        if (FMMOPacket.TryDeserialize(data, accumulated.Count, out var pkt, out consumed))
                        {
                            // Handle packet
                            OnPacketReceived?.Invoke(pkt);
                            accumulated.RemoveRange(0, consumed);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine("Receive error: " + e.Message);
                    break;
                }
            }
        }

        public void Send(FMMOPacket pkt)
        {
            if (!IsConnected) return;
            byte[] data = pkt.Serialize();
            if (data != null)
                _stream.Write(data, 0, data.Length);
        }

        public void Disconnect()
        {
            _running = false;
            _stream?.Close();
            _client?.Close();
        }

        public event Action<FMMOPacket> OnPacketReceived;
    }
}
