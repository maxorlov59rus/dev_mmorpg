// SRPAuth.h — SRP-6a серверная часть (заглушка)
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace dm {
namespace auth {

class SRPAuth {
public:
    void Init(const std::string &username, const std::string &password);
    std::vector<uint8_t> GetSalt() const;
    std::vector<uint8_t> GetPublicKey() const;
    bool VerifyProof(const std::vector<uint8_t> &clientProof);
    std::vector<uint8_t> ComputeServerProof() const;
private:
    std::string m_username;
    std::vector<uint8_t> m_salt;
    std::vector<uint8_t> m_pubkey;
};

} // namespace auth
} // namespace dm
