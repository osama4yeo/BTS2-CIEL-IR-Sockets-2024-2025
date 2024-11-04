#pragma once

enum class Player : uint8_t
{ 
	None,
	X,
	O
};

std::string_view PlayerToString(Player player);