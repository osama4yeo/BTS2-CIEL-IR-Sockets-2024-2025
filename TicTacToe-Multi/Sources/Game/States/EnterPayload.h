#pragma once

#include <variant>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/String.hpp>

struct Empty{};

enum class GameResult
{
	Draw,
	LocalPlayerWon,
	OpponentPlayerWon
};

std::string_view GameResultToString(GameResult gameResult);

struct Error
{
	sf::String error;
	bool isFatal;

	Error(const sf::String& e, bool fatal = false);
	Error(sf::String&& e, bool fatal = false);
};

using EnterPayload = std::variant<Empty, sf::IpAddress, GameResult, Error>;
