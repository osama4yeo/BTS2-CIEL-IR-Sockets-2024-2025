#pragma once

#include <string>
#include <vector>
#include <SFML/Network.hpp>

#include "GameInfo.h"
#include "Event.h"

class NetworkDiscovery
{
public:
	using EventDiscoverServer		= Event<void(const GameInfo& gameInfo)>;
	using EventUndiscoverServer		= Event<void(const GameInfo& gameInfo)>;


									NetworkDiscovery(const std::string& localServerName);
									~NetworkDiscovery();

	bool							Init();
	void							Term();

	void							StartBroadcast();
	void							StopBroadcast();

	void							Update();

	const std::vector<GameInfo>&	GetDiscoveredServers() const;

	EventDiscoverServer&			GetEventDiscoverServer();
	EventUndiscoverServer&			GetEventUndiscoverServer();

private:

	void							AddServerOrUpdate(const sf::IpAddress& ipAddress, const std::string& serverName);
	void							RemoveExpiredServers();

	sf::UdpSocket					_socket;
	sf::SocketSelector				_socketSelector;
	
	bool							_isBroadcastEnabled;
	uint64_t						_lastDeclareGameServerTimeMs;

	std::string						_localServerName;
	std::vector<GameInfo>			_discoveredServers;

	EventDiscoverServer				_eventDiscoverServer;
	EventUndiscoverServer			_eventUndiscoverServer;
};