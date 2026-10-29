// SRPAuth.cs — заглушка SRP-6a для Unity-клиента
using System;

namespace DarkMatter.Network
{
    public class SRPAuth
    {
        public string Username;
        public byte[] Salt;
        public byte[] Verifier;

        public void Init(string username, string password)
        {
            Username = username;
            // TODO: реализовать SRP-6a (RFC 5054)
            Salt = new byte[16];
            Verifier = new byte[32];
        }

        public byte[] ComputeClientProof(byte[] serverB)
        {
            // TODO
            return new byte[32];
        }

        public bool VerifyServerProof(byte[] serverProof)
        {
            // TODO
            return true;
        }
    }
}
