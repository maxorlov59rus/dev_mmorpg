#include "common/network/UdpListener.h"
#include "common/config/Config.h"
#include "common/Logger.h"
#include "common/database/DbPool.h"
#include "game/Rates/RateMgr.h"
#include "game/Time/RealTimeMgr.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    // Try multiple config paths
    std::vector<std::string> configPaths;
    if (argc > 1) configPaths.push_back(argv[1]);
    configPaths.push_back("config/worldserver.conf");
    configPaths.push_back("../config/worldserver.conf");
    configPaths.push_back("../../config/worldserver.conf");

    // Also try next to executable
    try {
        fs::path exePath(argv[0]);
        fs::path exeDir = exePath.parent_path();
        configPaths.push_back((exeDir / "config" / "worldserver.conf").string());
        configPaths.push_back((exeDir / ".." / "config" / "worldserver.conf").string());
    } catch (...) {}

    std::string configPath;
    bool configLoaded = false;
    for (const auto &p : configPaths) {
        std::ifstream test(p);
        if (test.is_open()) {
            configPath = p;
            configLoaded = dm::common::config::Config::Instance().Load(p);
            test.close();
            break;
        }
    }

    if (!configLoaded) {
        std::cerr << "ERROR: No config file found. Server cannot start without configuration.\n";
        std::cerr << "Searched paths:\n";
        for (const auto &p : configPaths) std::cerr << "  " << p << "\n";
        std::cerr << "Usage: " << argv[0] << " [path_to_worldserver.conf]\n";
        return 1;
    } else {
        std::cout << "Loaded config from: " << configPath << "\n";
    }

    // Initialize real time sync
    dm::RealTimeMgr::Instance().Init();
    auto &rt = dm::RealTimeMgr::Instance();
    std::cout << "Server time: " << rt.GetFormattedDate() << " " << rt.GetFormattedTime() << "\n";
    std::cout << "Weekend: " << (rt.IsWeekend() ? "Yes" : "No") << "\n";

    // Load server rates from config
    dm::RateMgr::Instance().LoadFromConfig();
    std::cout << "Server rates loaded. XP Rate: " << dm::RateMgr::Instance().GetXPRate() << "x";
    if (dm::RateMgr::Instance().IsWeekend()) {
        std::cout << " (Weekend bonus: x" << dm::RateMgr::Instance().GetWeekendXPMultiplier() << ")";
    }
    std::cout << "\n";

    // Ensure logs directory exists
    try { fs::create_directories("logs"); } catch (...) {}

    dm::common::Logger::Instance().Init("logs/worldserver.log");

    auto& log = dm::common::Logger::Instance();
    log.Info("BLACKMATTER TEAN PROJECT 2026");
    log.Info("World server starting...");

    dm::net::UdpListener udp(8093);
    if (!udp.Start()) {
        log.Error("Failed to start UDP listener on port 8093");
        return 1;
    }
    log.Info("World server started on UDP port 8093");
    log.Info("Server running. Press Ctrl+C to stop.");

    char buf[1024];
    while (true) {
        int received = udp.Receive(buf, sizeof(buf));
        if (received > 0) {
            log.Debug("Received " + std::to_string(received) + " bytes");
            udp.Send(buf, received, "127.0.0.1", 8093);
        }
    }
    udp.Stop();
    log.Info("World server stopped.");
    return 0;
}
