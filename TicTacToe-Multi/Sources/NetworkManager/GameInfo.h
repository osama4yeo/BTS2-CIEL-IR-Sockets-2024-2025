#pragma once

#include <string>
#include <SFML/Network.hpp>

struct GameInfo
{
    sf::IpAddress       serverIP;
    std::string         serverName;
    uint64_t            lastTimeReceivedMs;
};