#include "pch.h"

#include <windows.h>
#include <Lmcons.h>

#include "Game.h"
#include "Game/States/MenuState.h"
#include "Game/States/WaitingOpponentState.h"
#include "Game/States/ConnectingState.h"
#include "Game/States/PlayingState.h"
#include "Game/States/GameResultState.h"
#include "Game/States/ErrorState.h"

Game::Game()
	: _currentState(GameState::Menu)
	, _networkDiscovery(GetWindowsUsername())
	, _networkGame(GetWindowsUsername())
	, _window(sf::VideoMode(static_cast<unsigned int>(WindowSize), static_cast<unsigned int>(WindowSize) + 50), "Tic-Tac-Toe en réseau")
{
	bool loadingFontResult = _font.loadFromFile("arial.ttf");

	_states[static_cast<size_t>(GameState::Menu)].reset(new MenuState(this));
	_states[static_cast<size_t>(GameState::WaitingOpponent)].reset(new WaitingOpponentState(this));
	_states[static_cast<size_t>(GameState::Connecting)].reset(new ConnectingState(this));
	_states[static_cast<size_t>(GameState::Playing)].reset(new PlayingState(this));
	_states[static_cast<size_t>(GameState::GameResult)].reset(new GameResultState(this));
	_states[static_cast<size_t>(GameState::Error)].reset(new ErrorState(this));

	for(auto& state : _states)
	{
		if(!state)
		{
			std::cerr << "The state is null" << std::endl;
			ChangeState(GameState::Error, Error("Erreur lors de l'initialisation des states", true));
			return;
		}
	}

	if(!loadingFontResult)
	{
		std::cerr << "Fail to load the font" << std::endl;
		ChangeState(GameState::Error, Error("Erreur lors du chargement de la police", true));
		return;
	}

	if(!_networkDiscovery.Init())
	{
		ChangeState(GameState::Error, Error("Erreur lors de l'initialisation réseau", true));
		return;
	}

	_states[static_cast<size_t>(_currentState)]->Enter(EnterPayload());
}

Game::~Game()
{

}

void Game::Run()
{
	while(_window.isOpen())
	{
		HandleEvents();
		Update();
	}
}

sf::RenderWindow& Game::GetWindow()
{
	return _window;
}

sf::Font& Game::GetFont()
{
	return _font;
}

NetworkGame& Game::GetNetworkGame()
{
	return _networkGame;
}

NetworkDiscovery& Game::GetNetworkDiscovery()
{
	return _networkDiscovery;
}

void Game::ChangeState(GameState gameState, const EnterPayload& payload)
{
	std::cout << "Change state from " << GameStateToString(_currentState) << " to " << GameStateToString(gameState) << std::endl;
	std::cout << "GameState " << GameStateToString(_currentState) << "::Exit" << std::endl;

	_states[static_cast<size_t>(_currentState)]->Exit(gameState);
	_currentState = gameState;

	std::cout << "GameState " << GameStateToString(_currentState) << "::Enter" << std::endl;
	_states[static_cast<size_t>(_currentState)]->Enter(payload);
}

EventMouseClick& Game::GetEventMouseClick()
{
	return _eventMouseClick;
}

void Game::Update()
{
	_networkDiscovery.Update();

	_window.clear(sf::Color::White);
	_states[static_cast<size_t>(_currentState)]->Update();
	_window.display();
}

void Game::HandleEvents()
{
	sf::Event event;

	while(_window.pollEvent(event))
	{
		if(event.type == sf::Event::Closed)
		{
			std::cout << "Event[Close]" << std::endl;
			_window.close();
		}
		else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			HandleMouseClick(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
		}
	}
}

void Game::HandleMouseClick(float x, float y)
{
	_eventMouseClick.Invoke(x, y);

	std::cout << "Event[MouseClick] (x: " << x << ", y: " << y << ")" << std::endl;
}

std::string Game::GetWindowsUsername()
{
	char username[UNLEN + 1]{ 0 };
	DWORD usernameLength = sizeof(username);
	
	if(GetUserNameA(username, &usernameLength) == 0)
	{
		std::cerr << "Fail get the username GetUserNameA, GetLastError = " << GetLastError() << std::endl;
	}
	else
	{
		// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea
		// If the function succeeds, the return value is a nonzero value, and the variable pointed
		// to by lpnSize contains the number of TCHARs copied to the buffer specified by lpBuffer,
		// <b>including the terminating null character.</b>

		if(usernameLength > 0)
		{
			--usernameLength;
		}

		std::cout << "Get the username = " << username << " (" << usernameLength << ")" << std::endl;
	}

	return std::string(username, usernameLength);
}

