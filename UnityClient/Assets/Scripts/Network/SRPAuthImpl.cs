// SRPAuthImpl.cs — базовая реализация SRP-6a
using System;
using System.Numerics;
using System.Security.Cryptography;
using System.Text;
#if UNITY_2022_1_OR_NEWER
using Unity.Scripting.LifecycleManagement;
#endif

namespace DarkMatter.Network
{
#if UNITY_2022_1_OR_NEWER
    [NoAutoStaticsCleanup]
#endif
    public class SRPAuthImpl
    {
        private static readonly BigInteger N = BigInteger.Parse("0xAC6BDB41324A9A9BF166DE5E1389582FAF72B6651987EE07FC3192943DB56050A37329CBB4A099ED8193E0757767A13DD52312AB4B03310DCD7F48A9DA04FD50E8083969EDB767B0CF6095179A163AB3661A05FBD5FAAAE82918A9962F0B93B855F97993EC975EEAA80D740ADBF4FF747359D041D5C33EA71D281E446B14773BCA97B43A23FB801676BD207A436C6481F1D2B9078717461A5B9D32E688F87748544523B524B0D57D5EA77A2775D2ECFA032CFBDBF378E294865B0D38E4F70B704024F09390F7B7B5E0E3D9A8F5B3D87E83D8485C37A30EDC00678A38C7A6225E8B30C60B53B2C85A529B5D2E82473F2A739333476832BC78E08F3C458CE5A97C425F505E4F0A787190026059B9E64B4969E2E0EFD65889A5BF6F806455E936F09A5398F04F05A8E377C3D9006940BB5C3C08929D842AA32DB3A45E7ECE64F5DB9779AE6031D952927B8212DAF8C2B06C9B2F838CD5677F0A6", System.Globalization.NumberStyles.HexNumber);
        private static readonly BigInteger g = 2;

        private BigInteger a;
        private BigInteger A;
        private BigInteger B;
        private BigInteger u;
        private BigInteger S;
        private byte[] salt;
        private string username;

        public void Init(string username, string password, byte[] serverSalt, BigInteger serverB)
        {
            this.username = username;
            salt = serverSalt;
            B = serverB;
            using (var sha = SHA256.Create())
            {
                var saltPlusUser = new byte[salt.Length + Encoding.UTF8.GetByteCount(username)];
                Buffer.BlockCopy(salt, 0, saltPlusUser, 0, salt.Length);
                var userBytes = Encoding.UTF8.GetBytes(username); Buffer.BlockCopy(userBytes, 0, saltPlusUser, salt.Length, userBytes.Length);
                var xHash = sha.ComputeHash(saltPlusUser);
                var x = new BigInteger(xHash, isUnsigned: true, isBigEndian: true);

                byte[] aBytes = new byte[32]; RandomNumberGenerator.Fill(aBytes); a = new BigInteger(aBytes, isUnsigned: true, isBigEndian: true);
                A = BigInteger.ModPow(g, a, N);

                var ab = new byte[A.ToByteArray().Length + B.ToByteArray().Length];
                Buffer.BlockCopy(A.ToByteArray(), 0, ab, 0, A.ToByteArray().Length);
                Buffer.BlockCopy(B.ToByteArray(), 0, ab, A.ToByteArray().Length, B.ToByteArray().Length);
                var uHash = sha.ComputeHash(ab);
                u = new BigInteger(uHash, isUnsigned: true, isBigEndian: true);

                var gx = BigInteger.ModPow(g, x, N);
                var baseVal = (B - 3 * gx) % N;
                if (baseVal < 0) baseVal += N;
                var exp = (a + u * x) % (N - 1);
                S = BigInteger.ModPow(baseVal, exp, N);
            }
        }

        public BigInteger GetClientPublic() => A;

        public byte[] ComputeClientProof()
        {
            using (var sha = SHA256.Create())
            {
                var HN = sha.ComputeHash(N.ToByteArray());
                var Hg = sha.ComputeHash(g.ToByteArray());
                var Hxor = new byte[HN.Length];
                for (int i = 0; i < HN.Length; i++) Hxor[i] = (byte)(HN[i] ^ Hg[i]);
                var Hu = sha.ComputeHash(Encoding.UTF8.GetBytes(username));
                var Hs = sha.ComputeHash(salt);
                var HA = sha.ComputeHash(A.ToByteArray());
                var HB = sha.ComputeHash(B.ToByteArray());
                var K = sha.ComputeHash(S.ToByteArray());

                var concat = new byte[Hxor.Length + Hu.Length + Hs.Length + HA.Length + HB.Length + K.Length];
                int off = 0;
                Buffer.BlockCopy(Hxor, 0, concat, off, Hxor.Length); off += Hxor.Length;
                Buffer.BlockCopy(Hu, 0, concat, off, Hu.Length); off += Hu.Length;
                Buffer.BlockCopy(Hs, 0, concat, off, Hs.Length); off += Hs.Length;
                Buffer.BlockCopy(HA, 0, concat, off, HA.Length); off += HA.Length;
                Buffer.BlockCopy(HB, 0, concat, off, HB.Length); off += HB.Length;
                Buffer.BlockCopy(K, 0, concat, off, K.Length); off += K.Length;
                return sha.ComputeHash(concat);
            }
        }

        public bool VerifyServerProof(byte[] serverProof, byte[] clientProof)
        {
            using (var sha = SHA256.Create())
            {
                var HA = sha.ComputeHash(A.ToByteArray());
                var M1 = clientProof;
                var K = sha.ComputeHash(S.ToByteArray());
                var concat = new byte[HA.Length + M1.Length + K.Length];
                Buffer.BlockCopy(HA, 0, concat, 0, HA.Length);
                Buffer.BlockCopy(M1, 0, concat, HA.Length, M1.Length);
                Buffer.BlockCopy(K, 0, concat, HA.Length + M1.Length, K.Length);
                var M2 = sha.ComputeHash(concat);
                return M2.Length == serverProof.Length && M2.AsSpan().SequenceEqual(serverProof);
            }
        }
    }
}
