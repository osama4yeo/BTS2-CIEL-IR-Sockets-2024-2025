#pragma once

#include "EnterPayload.h"

class Game;

enum class GameState : uint8_t
{
	Menu,
	WaitingOpponent,
	Connecting,
	Playing,
	GameResult,
	Error,
	Count
};

std::string_view GameStateToString(GameState gameState);

class BaseState
{
public:
								BaseState(Game* pGame);
	virtual						~BaseState();

	virtual void				Enter(const EnterPayload& enterPayload) = 0;
	virtual void				Update() = 0;
	virtual void				Exit(GameState nextState) = 0;

protected:

	Game*						_pGame;
};