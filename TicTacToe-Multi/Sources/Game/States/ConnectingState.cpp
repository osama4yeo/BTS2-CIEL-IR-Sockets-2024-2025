#include "pch.h"
#include "ConnectingState.h"
#include "Game/Game.h"

ConnectingState::ConnectingState(Game* pGame)
	: BaseState(pGame)
	, _delegateMouseClick(std::bind_front(&ConnectingState::HandleMouseClick, this))
	, _titleText("Tic-Tac-Toe en réseau", pGame->GetFont(), 40)
	, _informationText("Connection", pGame->GetFont(), 30)
	, _backText("Retour", pGame->GetFont(), 30)
{
	_titleText.setFillColor(sf::Color::Black);
	_titleText.setPosition(WindowSize / 2 - _titleText.getLocalBounds().width / 2, 50);

	_backText.setFillColor(sf::Color::Black);
	_backText.setPosition(50, 250);
}

ConnectingState::~ConnectingState()
{

}

void ConnectingState::Enter(const EnterPayload& enterPayload)
{
	if(!std::holds_alternative<sf::IpAddress>(enterPayload))
	{
		_pGame->ChangeState(GameState::Error, Error("Data invalide dans ConnectingState::Enter"));
		return;
	}

	sf::IpAddress ipAddress = std::get<sf::IpAddress>(enterPayload);

	std::cout << "ConnectingState connect to " << ipAddress.toString() << std::endl;

	_pGame->GetEventMouseClick().AddListener(_delegateMouseClick);

	_informationText.setString("Connection à " + ipAddress.toString());
	_informationText.setFillColor(sf::Color::Blue);
	_informationText.setPosition(WindowSize / 2 - _informationText.getLocalBounds().width / 2, 150);

	if(!_pGame->GetNetworkGame().JoinGame(ipAddress))
	{
		_pGame->ChangeState(GameState::Error, Error("Impossible de se connecter à l'adresse " + ipAddress.toString()));
	}
}

void ConnectingState::Update()
{
	_pGame->GetWindow().draw(_titleText);
	_pGame->GetWindow().draw(_informationText);
	_pGame->GetWindow().draw(_backText);

	if(_pGame->GetNetworkGame().IsConnectionEstablish())
	{
		sf::Socket::Status status = _pGame->GetNetworkGame().WaitingAnOpponentName();

		switch(status)
		{
		case sf::Socket::Done:
		{
			if(_pGame->GetNetworkGame().HasReceivedOpponentName())
			{
				_pGame->ChangeState(GameState::Playing);
			}
			else
			{
				_pGame->ChangeState(GameState::Error, Error("Le nom de l'autre joueur n'a pas pu être reçu"));
			}
			break;
		}

		case sf::Socket::NotReady:
		{
			break;
		}

		default:
		{
			_pGame->ChangeState(GameState::Error, Error("Le nom de l'autre joueur n'a pas pu être reçu"));
			break;
		}
		}
	}
}

void ConnectingState::Exit(GameState nextState)
{
	if(nextState != GameState::Playing)
	{
		std::cout << "Game not properly initialized, Reset network" << std::endl;

		_pGame->GetNetworkGame().StopPlaying();
	}

	_pGame->GetEventMouseClick().RemoveListener(_delegateMouseClick);
}

void ConnectingState::HandleMouseClick(float x, float y)
{
	if(_backText.getGlobalBounds().contains(x, y))
	{
		std::cout << "Back clicked" << std::endl;

		_pGame->ChangeState(GameState::Menu);
	}
}
