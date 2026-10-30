#include "game/Auth/GoogleAuthMgr.h"
#include <unordered_map>
#include <string>

namespace dm {

static std::unordered_map<std::string, GoogleAccount> s_accounts;
static std::unordered_map<std::string, GoogleAuthToken> s_tokens;
static GoogleAuthConfig s_config = {
    "YOUR_GOOGLE_CLIENT_ID.apps.googleusercontent.com",
    "YOUR_GOOGLE_CLIENT_SECRET",
    "http://localhost:8080/auth/google/callback",
    true,
    "openid email profile",
    24,
    30
};

GoogleAuthMgr &GoogleAuthMgr::Instance() {
    static GoogleAuthMgr inst;
    return inst;
}

std::string GoogleAuthMgr::GetAuthUrl() const {
    if (!s_config.enabled) return "";
    return "https://accounts.google.com/o/oauth2/v2/auth?"
           "client_id=" + s_config.clientId +
           "&redirect_uri=" + s_config.redirectUri +
           "&response_type=code"
           "&scope=" + s_config.scope +
           "&access_type=offline"
           "&prompt=consent";
}

bool GoogleAuthMgr::HandleCallback(const std::string &code, const std::string &state, GoogleAccount &outAccount) {
    if (!s_config.enabled) return false;
    if (code.empty()) return false;

    // TODO: Exchange code for tokens via Google OAuth API
    // TODO: Fetch user info from Google
    // TODO: Create or update GoogleAccount
    // TODO: Generate session token

    // Placeholder: create account from code
    outAccount.googleId = "google_" + code.substr(0, 8);
    outAccount.email = outAccount.googleId + "@gmail.com";
    outAccount.name = "Google User";
    outAccount.pictureUrl = "";
    outAccount.locale = "ru";
    outAccount.accountId = 0;
    outAccount.createdAt = 0;
    outAccount.lastLoginAt = 0;

    s_accounts[outAccount.googleId] = outAccount;
    return true;
}

bool GoogleAuthMgr::ValidateToken(const std::string &accessToken) const {
    if (accessToken.empty()) return false;
    // TODO: Validate token with Google API
    return true;
}

bool GoogleAuthMgr::RefreshToken(const std::string &refreshToken, GoogleAuthToken &outToken) {
    if (refreshToken.empty()) return false;
    // TODO: Refresh token via Google OAuth API
    return true;
}

GoogleAccount *GoogleAuthMgr::GetAccount(const std::string &googleId) {
    auto it = s_accounts.find(googleId);
    if (it == s_accounts.end()) return nullptr;
    return &it->second;
}

GoogleAccount *GoogleAuthMgr::GetAccountByEmail(const std::string &email) {
    for (auto &kv : s_accounts) {
        if (kv.second.email == email) return &kv.second;
    }
    return nullptr;
}

bool GoogleAuthMgr::LinkAccount(uint64_t accountId, const std::string &googleId) {
    auto it = s_accounts.find(googleId);
    if (it == s_accounts.end()) return false;
    it->second.accountId = accountId;
    return true;
}

bool GoogleAuthMgr::UnlinkAccount(uint64_t accountId) {
    for (auto &kv : s_accounts) {
        if (kv.second.accountId == accountId) {
            kv.second.accountId = 0;
            return true;
        }
    }
    return false;
}

GoogleAuthConfig GoogleAuthMgr::GetConfig() const {
    return s_config;
}

void GoogleAuthMgr::SetConfig(const GoogleAuthConfig &config) {
    s_config = config;
}

bool GoogleAuthMgr::IsEnabled() const {
    return s_config.enabled;
}

void GoogleAuthMgr::LoadConfig() {
    // TODO: Load from database
}

void GoogleAuthMgr::LoadAccounts() {
    // TODO: Load from database
}

} // namespace dm