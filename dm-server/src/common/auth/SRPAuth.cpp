#include "common/auth/SRPAuth.h"
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <cstring>
#include <vector>
#include <string>

namespace dm {
namespace auth {

static const char *SRP_N_HEX =
    "AC6BDB41324A9A9BF166DE5E1389582FAF72B6651987EE07FC3192943DB56050"
    "A37329CBB4A099ED8193E0757767A13DD52312AB4B03310DCD7F48A9DA04FD50E"
    "8083969EDB767B0CF6095179A163AB3661A05FBD5FAAAE82918A9962F0B93B855F"
    "97993EC975EEAA80D740ADBF4FF747359D041D5C33EA71D281E446B14773BCA97B"
    "43A23FB801676BD207A436C6481F1D2B9078717461A5B9D32E688F87748544523B"
    "524B0D57D5EA77A2775D2ECFA032CFBDBF378E294865B0D38E4F70B704024F0939"
    "0F7B7B5E0E3D9A8F5B3D87E83D8485C37A30EDC00678A38C7A6225E8B30C60B53B"
    "2C85A529B5D2E82473F2A739333476832BC78E08F3C458CE5A97C425F505E4F0A7"
    "87190026059B9E64B4969E2E0EFD65889A5BF6F806455E936F09A5398F04F05A8E"
    "377C3D9006940BB5C3C08929D842AA32DB3A45E7ECE64F5DB9779AE6031D952927"
    "B8212DAF8C2B06C9B2F838CD5677F0A6";
static const int SRP_G = 2;
static const int SRP_K = 3;

static BIGNUM *N = nullptr;
static BIGNUM *g = nullptr;
static BIGNUM *k = nullptr;

static void InitParams() {
    if (N) return;
    BN_CTX *ctx = BN_CTX_new();
    N = BN_new();
    BN_hex2bn(&N, SRP_N_HEX);
    g = BN_new();
    BN_set_word(g, SRP_G);
    k = BN_new();
    BN_set_word(k, SRP_K);
    BN_CTX_free(ctx);
}

static std::vector<uint8_t> BnToBytes(const BIGNUM *bn) {
    int len = BN_num_bytes(bn);
    std::vector<uint8_t> buf(len);
    BN_bn2bin(bn, buf.data());
    return buf;
}

static BIGNUM *BytesToBn(const std::vector<uint8_t> &buf) {
    BIGNUM *bn = BN_new();
    BN_bin2bn(buf.data(), buf.size(), bn);
    return bn;
}

static void Sha256(const std::vector<uint8_t> &data, std::vector<uint8_t> &out) {
    out.resize(32);
    SHA256(data.data(), data.size(), out.data());
}

void SRPAuth::Init(const std::string &username, const std::string &password) {
    InitParams();
    m_username = username;
    m_password = password;

    m_salt.resize(16);
    RAND_bytes(m_salt.data(), 16);

    std::vector<uint8_t> saltPlusUser;
    saltPlusUser.insert(saltPlusUser.end(), m_salt.begin(), m_salt.end());
    saltPlusUser.insert(saltPlusUser.end(), username.begin(), username.end());
    std::vector<uint8_t> xHash;
    Sha256(saltPlusUser, xHash);
    BIGNUM *x = BytesToBn(xHash);

    BIGNUM *v = BN_new();
    BN_CTX *ctx = BN_CTX_new();
    BN_mod_exp(v, g, x, N, ctx);
    m_verifier = BnToBytes(v);

    BN_free(x);
    BN_free(v);
    BN_CTX_free(ctx);
}

std::vector<uint8_t> SRPAuth::GetSalt() const { return m_salt; }

std::vector<uint8_t> SRPAuth::GetPublicKey() {
    InitParams();
    BIGNUM *b = BN_new();
    BN_rand(b, 256, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);

    BIGNUM *v = BytesToBn(m_verifier);

    BIGNUM *gv = BN_new();
    BN_CTX *ctx = BN_CTX_new();
    BN_mod_exp(gv, g, b, N, ctx);

    BIGNUM *kv = BN_new();
    BN_mul(kv, k, v, ctx);

    BIGNUM *B = BN_new();
    BN_mod_add(B, kv, gv, N, ctx);

    m_privateB = b;
    m_publicB = B;

    auto res = BnToBytes(B);

    BN_free(v);
    BN_free(gv);
    BN_free(kv);
    BN_CTX_free(ctx);
    return res;
}

bool SRPAuth::VerifyProof(const std::vector<uint8_t> &clientProof) {
    // TODO: verify M1
    return true;
}

std::vector<uint8_t> SRPAuth::ComputeServerProof() const {
    // TODO: compute M2
    return {};
}

} // namespace auth
} // namespace dm
