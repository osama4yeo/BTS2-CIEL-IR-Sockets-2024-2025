#pragma once

#include <SFML/Network.hpp>

enum class PacketType : uint8_t
{
	None,
	PlayerName,
	Move
};

std::string_view PacketTypeToString(PacketType packetType);

class NetworkGame
{
public:
								NetworkGame(const std::string& localPlayerName);
								~NetworkGame();

								// Connections

	bool						HostGame();
	bool						WaitingAnOpponent();
	sf::Socket::Status			WaitingAnOpponentName();

	bool						JoinGame(const sf::IpAddress& serverIP);

	void						StopPlaying();

								// Moves

	bool						SendMove(int row, int col);
	sf::Socket::Status			WaitingReceiveMove(int& row, int& col);

								// Status

	bool						IsServer() const;
	bool						IsConnectionEstablish() const;
	bool						HasReceivedOpponentName() const;

								// Player names

	const std::string&			GetLocalPlayerName() const;
	const std::string&			GetOpponentName() const;

private:

	bool						SendLocalPlayerName();
	sf::Socket::Status			TryToReceivePacket(PacketType packetTypeExpect);

	sf::TcpSocket				_socket;
	sf::TcpListener				_listener;
	sf::SocketSelector			_selector;
	sf::Packet					_receivedPacket;

	bool						_isServer;
	bool						_isConnectionEstablish;
	bool						_hasReceivedOpponentName;

	std::string					_localPlayerName;
	std::string					_opponentName;
};