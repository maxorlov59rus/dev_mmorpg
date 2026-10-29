cmake_minimum_required(VERSION 3.16)
project(dm_server CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

file(GLOB_RECURSE SERVER_SRC
    "src/**/*.cpp"
    "src/**/*.h"
)

list(REMOVE_ITEM SERVER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Vendor/VendorMgr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Trade/TradeMgr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Quests/QuestSystem_Level10.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Quests/QuestGossipHandlers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/LFG/LFGMgr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/AI/SmartAI.cpp"
)

add_executable(dm_server ${SERVER_SRC} src/main.cpp)

target_include_directories(dm_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/../proto
)

install(TARGETS dm_server DESTINATION bin)
