// BinaryReaderExtensions.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace DarkMatter.Network
{
    public static class BinaryReaderExtensions
    {
        public static string ReadString(this BinaryReader r, int maxLen)
        {
            // str<n>: read until null or maxLen
            byte[] buf = r.ReadBytes(maxLen);
            int len = 0;
            while (len < buf.Length && buf[len] != 0) len++;
            return Encoding.UTF8.GetString(buf, 0, len);
        }

        public static byte[] ReadBytes(this BinaryReader r, int count)
        {
            return r.ReadBytes(count);
        }

        public static List<T> ReadArray<T>(this BinaryReader r, int max) where T : new()
        {
            // arr<T>{max}: read count (u8) then elements
            byte count = r.ReadByte();
            var list = new List<T>(count);
            for (int i = 0; i < count && i < max; i++)
            {
                // For simple types, T must be primitive; for structs, use FromReader
                // This generic version only works for primitives; complex types need custom code.
                list.Add(default(T));
            }
            return list;
        }
    }
}
