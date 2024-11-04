#include "pch.h"
#include "EnterPayload.h"

Error::Error(const sf::String& e, bool fatal) : error(e), isFatal(fatal)
{

}

Error::Error(sf::String&& e, bool fatal) : error(std::move(e)), isFatal(fatal)
{

}

std::string_view GameResultToString(GameResult gameResult)
{
	switch(gameResult)
	{
	case GameResult::Draw:					return "Draw"sv;
	case GameResult::LocalPlayerWon:		return "LocalPlayerWon"sv;
	case GameResult::OpponentPlayerWon:		return "OpponentPlayerWon"sv;
	default:								return "Unknown"sv;
	}
}
