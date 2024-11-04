#include "pch.h"
#include "MenuState.h"
#include "Game/Game.h"

MenuState::MenuState(Game* pGame)
	: BaseState(pGame)
	, _delegateMouseClick(std::bind_front(&MenuState::HandleMouseClick, this))
	, _delegateDiscoverServer(std::bind_front(&MenuState::HandleDiscoverServer, this))
	, _delegateUndiscoverServer(std::bind_front(&MenuState::HandleUndiscoverServer, this))
	, _titleText("Tic-Tac-Toe en réseau", pGame->GetFont(), 40)
	, _informationText("Héberger une partie", pGame->GetFont(), 30)
	, _backText("Parties disponibles:", pGame->GetFont(), 30)
{
	_titleText.setFillColor(sf::Color::Black);
	_titleText.setPosition(WindowSize / 2 - _titleText.getLocalBounds().width / 2, 50);

	_informationText.setFillColor(sf::Color::Blue);
	_informationText.setPosition(WindowSize / 2 - _informationText.getLocalBounds().width / 2, 150);

	_backText.setFillColor(sf::Color::Black);
	_backText.setPosition(50, 250);

	_pGame->GetNetworkDiscovery().GetEventDiscoverServer().AddListener(_delegateDiscoverServer);
	_pGame->GetNetworkDiscovery().GetEventUndiscoverServer().AddListener(_delegateUndiscoverServer);
}

MenuState::~MenuState()
{
	_pGame->GetNetworkDiscovery().GetEventDiscoverServer().RemoveListener(_delegateDiscoverServer);
	_pGame->GetNetworkDiscovery().GetEventUndiscoverServer().RemoveListener(_delegateUndiscoverServer);
}

void MenuState::Enter(const EnterPayload& enterPayload)
{
	_pGame->GetNetworkGame().StopPlaying();
	_pGame->GetEventMouseClick().AddListener(_delegateMouseClick);
}

void MenuState::Update()
{
	_pGame->GetWindow().draw(_titleText);
	_pGame->GetWindow().draw(_informationText);
	_pGame->GetWindow().draw(_backText);

	float index = 0.f;

	for(AvailableGames& availableGame : _availableGames)
	{
		availableGame._gameText.setPosition(50, 300 + index * 30);
		_pGame->GetWindow().draw(availableGame._gameText);

		index++;
	}
}

void MenuState::Exit(GameState nextState)
{
	_pGame->GetEventMouseClick().RemoveListener(_delegateMouseClick);
}

void MenuState::HandleMouseClick(float x, float y)
{
	if(_informationText.getGlobalBounds().contains(x, y))
	{
		std::cout << "Host a game" << std::endl;
		_pGame->ChangeState(GameState::WaitingOpponent, EnterPayload());
	}

	for(AvailableGames& availableGame : _availableGames)
	{
		if(availableGame._gameText.getGlobalBounds().contains(x, y))
		{
			std::cout << "Connecting to " << availableGame._gameInfo.serverIP << ", " << availableGame._gameInfo.serverName << " server" << std::endl;
			_pGame->ChangeState(GameState::Connecting, EnterPayload(availableGame._gameInfo.serverIP));
			break;
		}
	}
}

void MenuState::HandleDiscoverServer(const GameInfo& gameInfo)
{
	AvailableGames& availableGame = _availableGames.emplace_back();

	std::string text = gameInfo.serverName + " - " + gameInfo.serverIP.toString();

	availableGame._gameInfo = gameInfo;
	availableGame._gameText = sf::Text(text, _pGame->GetFont(), 20);
	availableGame._gameText.setFillColor(sf::Color::Black);

	std::cout << "Creating UI for " << gameInfo.serverIP.toString() << ", " << gameInfo.serverName << " server" << std::endl;
}

void MenuState::HandleUndiscoverServer(const GameInfo& gameInfo)
{

	std::vector<AvailableGames>::iterator it = std::find_if(
		_availableGames.begin(),
		_availableGames.end(),
		[&gameInfo](AvailableGames& availableGame)
		{
			return availableGame._gameInfo.serverIP == gameInfo.serverIP;
		});

	if(it != _availableGames.end())
	{
		std::cout << "Removing " << gameInfo.serverIP << ", " << gameInfo.serverName << " server" << std::endl;
		_availableGames.erase(it);
	}
}