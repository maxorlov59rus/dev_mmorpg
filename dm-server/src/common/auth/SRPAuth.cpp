#include "common/auth/SRPAuth.h"

namespace dm {
namespace auth {

void SRPAuth::Init(const std::string &username, const std::string &password) {
    m_username = username;
    // TODO: SRP-6a init
}

std::vector<uint8_t> SRPAuth::GetSalt() const { return m_salt; }
std::vector<uint8_t> SRPAuth::GetPublicKey() const { return m_pubkey; }
bool SRPAuth::VerifyProof(const std::vector<uint8_t> &clientProof) { return true; }
std::vector<uint8_t> SRPAuth::ComputeServerProof() const { return {}; }

} // namespace auth
} // namespace dm
