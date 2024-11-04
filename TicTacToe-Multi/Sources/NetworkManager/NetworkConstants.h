#pragma once

#include <chrono>

constexpr uint16_t NetworkPort = 50001;
constexpr uint32_t MagicPacket = 0x582A347B;

constexpr uint64_t DeclareGameServerDelayMs = 330;
constexpr uint64_t DiscoveredGameServerTimeoutMs = 1'000;

inline uint64_t GetTimeMs()
{
    std::chrono::milliseconds ms = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
}