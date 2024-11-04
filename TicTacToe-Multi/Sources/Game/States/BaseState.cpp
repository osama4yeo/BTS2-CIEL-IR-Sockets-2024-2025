#include "pch.h"
#include "BaseState.h"

BaseState::BaseState(Game* pGame)
	: _pGame(pGame)
{
}

BaseState::~BaseState()
{
}

std::string_view GameStateToString(GameState gameState)
{
	switch(gameState)
	{
	case GameState::Menu:				return "Menu"sv;
	case GameState::WaitingOpponent:	return "WaitingOpponent"sv;
	case GameState::Connecting:			return "Connecting"sv;
	case GameState::Playing:			return "Playing"sv;
	case GameState::GameResult:			return "GameResult"sv;
	case GameState::Error:				return "Error"sv;
	default:							return "Unknown"sv;
	}
}
