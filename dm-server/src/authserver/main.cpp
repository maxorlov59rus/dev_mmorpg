#include "common/network/TcpListener.h"
#include "common/config/Config.h"
#include "common/Logger.h"
#include "common/database/DbPool.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char **argv) {
    const char *configPath = (argc > 1) ? argv[1] : "config/authserver.conf";
    auto& config = dm::common::config::Config::Instance();
    config.Load(configPath);

    // Read logging settings from config
    std::string logFile = config.GetString("Logging", "file", "logs/authserver.log");
    std::string logLevelStr = config.GetString("Logging", "level", "INFO");
    bool logConsole = config.GetBool("Logging", "console", true);
    bool logFileOutput = config.GetBool("Logging", "file_output", true);
    auto logLevel = dm::common::Logger::Instance().StringToLevel(logLevelStr);

    // Initialize logging
    dm::common::Logger::Instance().Init(logFile, logLevel, logConsole, logFileOutput);

    auto& log = dm::common::Logger::Instance();
    log.Info("Auth server starting...");

    // Read network settings from config
    uint16_t clientPort = static_cast<uint16_t>(config.GetInt("Network", "client_port", 8091));

    // Read database settings from config
    std::string dbHost = config.GetString("Database", "host", "127.0.0.1");
    uint16_t dbPort = static_cast<uint16_t>(config.GetInt("Database", "port", 3306));
    std::string dbUser = config.GetString("Database", "user", "mmorpg");
    std::string dbPass = config.GetString("Database", "password", "mmorpg_pass");
    std::string dbName = config.GetString("Database", "database", "mmorpg_auth");
    size_t dbPoolSize = static_cast<size_t>(config.GetInt("Database", "pool_size", 5));

    // Initialize database pool
    log.Info("Initializing database pool...");
    dm::common::database::DbPool::Instance().Init(dbHost, dbPort, dbUser, dbPass, dbName, dbPoolSize);
    log.Info("Database pool initialized.");

    dm::net::TcpListener tcp(clientPort);
    if (!tcp.Start()) {
        log.Error("Failed to start TCP listener on port " + std::to_string(clientPort));
        return 1;
    }
    log.Info("Auth server started on TCP port " + std::to_string(clientPort));
    log.Info("Auth server running. Press Ctrl+C to stop.");

    while (true) {
        int clientFd = tcp.Accept();
        if (clientFd >= 0) {
            log.Info("Client connected");
            closesocket(clientFd);
        }
    }

    dm::common::database::DbPool::Instance().Shutdown();
    tcp.Stop();
    log.Info("Auth server stopped.");
    return 0;
}
