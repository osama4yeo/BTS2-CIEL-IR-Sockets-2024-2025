#pragma once

#include "Constants.h"
#include "NetworkManager/NetworkDiscovery.h"
#include "NetworkManager/NetworkGame.h"
#include "Game/States/BaseState.h"
#include "GameEvent.h"

class Game
{
public:
									Game();
									~Game();

	void							Run();

	sf::RenderWindow&				GetWindow();
	sf::Font&						GetFont();

	NetworkGame&					GetNetworkGame();
	NetworkDiscovery&				GetNetworkDiscovery();

	void							ChangeState(GameState gameState, const EnterPayload& payload = EnterPayload());

	EventMouseClick&				GetEventMouseClick();

private:

	void							Update();

	void							HandleEvents();
	void							HandleMouseClick(float x, float y);

private:

	static std::string				GetWindowsUsername();

	GameState						_currentState;
	std::unique_ptr<BaseState>		_states[static_cast<size_t>(GameState::Count)];

	NetworkDiscovery				_networkDiscovery;
	NetworkGame						_networkGame;

	sf::RenderWindow				_window;
	sf::Font						_font;

	EventMouseClick					_eventMouseClick;
};