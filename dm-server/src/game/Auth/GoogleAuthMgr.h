// GoogleAuthMgr.h — Авторизация через Google OAuth 2.0
#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

namespace dm {

struct GoogleAccount {
    std::string googleId;
    std::string email;
    std::string name;
    std::string pictureUrl;
    std::string locale;
    uint64_t accountId;
    uint32_t createdAt;
    uint32_t lastLoginAt;
};

struct GoogleAuthToken {
    uint64_t accountId;
    std::string googleId;
    std::string accessToken;
    std::string refreshToken;
    std::string idToken;
    std::string tokenType;
    uint32_t expiresAt;
    uint32_t createdAt;
    uint32_t updatedAt;
};

struct GoogleAuthConfig {
    std::string clientId;
    std::string clientSecret;
    std::string redirectUri;
    bool enabled;
    std::string scope;
    uint32_t tokenExpiryHours;
    uint32_t refreshTokenExpiryDays;
};

class GoogleAuthMgr {
public:
    static GoogleAuthMgr &Instance();

    // Авторизация
    std::string GetAuthUrl() const;
    bool HandleCallback(const std::string &code, const std::string &state, GoogleAccount &outAccount);
    bool ValidateToken(const std::string &accessToken) const;
    bool RefreshToken(const std::string &refreshToken, GoogleAuthToken &outToken);

    // Аккаунты
    GoogleAccount *GetAccount(const std::string &googleId);
    GoogleAccount *GetAccountByEmail(const std::string &email);
    bool LinkAccount(uint64_t accountId, const std::string &googleId);
    bool UnlinkAccount(uint64_t accountId);

    // Конфигурация
    GoogleAuthConfig GetConfig() const;
    void SetConfig(const GoogleAuthConfig &config);
    bool IsEnabled() const;

    // Загрузка данных
    void LoadConfig();
    void LoadAccounts();
};

} // namespace dm