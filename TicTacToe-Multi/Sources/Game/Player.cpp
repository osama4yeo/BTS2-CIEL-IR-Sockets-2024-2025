#include "pch.h"
#include "Player.h"

std::string_view PlayerToString(Player player)
{
	switch(player)
	{
	case Player::None:			return " "sv;
	case Player::X:				return "X"sv;
	case Player::O:				return "O"sv;
	default:					return "~"sv;
	}
}
