cmake_minimum_required(VERSION 3.16)
project(dm_server CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SERVER_SRC
    "src/**/*.cpp"
    "src/**/*.h"
    "src/**/*.c"
)

list(REMOVE_ITEM SERVER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Vendor/VendorMgr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Trade/TradeMgr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Quests/QuestSystem_Level10.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Quests/QuestGossipHandlers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/LFG/LFGMgr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/AI/SmartAI.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/authserver/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

# World server
list(REMOVE_ITEM SERVER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Items/ItemTemplateStore.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/Items/ItemTemplateStore.h"
)

add_executable(dm_server ${SERVER_SRC} src/main.cpp)
target_include_directories(dm_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/network
    ${CMAKE_CURRENT_SOURCE_DIR}/../proto
    "C:/Program Files/OpenSSL-Win64/include"
)
target_link_libraries(dm_server PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Auth server
file(GLOB_RECURSE AUTH_SRC
    "src/common/**/*.cpp"
    "src/common/**/*.h"
    "src/common/**/*.c"
    "src/authserver/main.cpp"
)
add_executable(dm_authserver ${AUTH_SRC} src/common/Logger.cpp)
target_include_directories(dm_authserver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/network
    ${CMAKE_CURRENT_SOURCE_DIR}/../proto
    "C:/Program Files/OpenSSL-Win64/include"
)
target_link_libraries(dm_authserver PRIVATE OpenSSL::SSL OpenSSL::Crypto)

install(TARGETS dm_server dm_authserver DESTINATION bin)
file(GLOB LUA_SRC "thirdparty/lua/src/*.c")
add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC "thirdparty/lua/src")
target_link_libraries(dm_server PRIVATE lua)
