// FMMOPacket.cs — зеркало C++ FMMOPacket
using System;
using System.Collections.Generic;

namespace DarkMatter.Network
{
    public class FMMOPacket
    {
        public ushort Opcode = 0;
        public List<byte> Payload = new List<byte>();

        public static bool TryDeserialize(byte[] data, int num, out FMMOPacket packet, out int consumed)
        {
            packet = null;
            consumed = 0;
            if (num < 4) return false;
            ushort frameSize = (ushort)(data[0] | (data[1] << 8));
            if (frameSize < 4 || frameSize > 512 * 1024) return false;
            if (num < frameSize) return false;
            packet = new FMMOPacket();
            packet.Opcode = (ushort)(data[2] | (data[3] << 8));
            int payloadLen = frameSize - 4;
            if (payloadLen > 0)
            {
                packet.Payload = new List<byte>(payloadLen);
                for (int i = 0; i < payloadLen; i++)
                    packet.Payload.Add(data[4 + i]);
            }
            consumed = frameSize;
            return true;
        }

        public byte[] Serialize()
        {
            int totalSize = 4 + Payload.Count;
            if (totalSize > 512 * 1024) return null;
            byte[] bytes = new byte[totalSize];
            bytes[0] = (byte)(totalSize & 0xFF);
            bytes[1] = (byte)((totalSize >> 8) & 0xFF);
            bytes[2] = (byte)(Opcode & 0xFF);
            bytes[3] = (byte)((Opcode >> 8) & 0xFF);
            if (Payload.Count > 0)
                Payload.CopyTo(bytes, 4);
            return bytes;
        }
    }
}
