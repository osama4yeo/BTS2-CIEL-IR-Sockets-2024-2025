#include "pch.h"
#include "WaitingOpponentState.h"
#include "Game/Game.h"

WaitingOpponentState::WaitingOpponentState(Game* pGame)
	: BaseState(pGame)
	, _delegateMouseClick(std::bind_front(&WaitingOpponentState::HandleMouseClick, this))
	, _titleText("Tic-Tac-Toe en réseau", pGame->GetFont(), 40)
	, _informationText("En attente d'un joueur...", pGame->GetFont(), 30)
	, _backText("Retour", pGame->GetFont(), 30)
{
	_titleText.setFillColor(sf::Color::Black);
	_titleText.setPosition(WindowSize / 2 - _titleText.getLocalBounds().width / 2, 50);

	_informationText.setFillColor(sf::Color::Blue);
	_informationText.setPosition(WindowSize / 2 - _informationText.getLocalBounds().width / 2, 150);

	_backText.setFillColor(sf::Color::Black);
	_backText.setPosition(50, 250);
}

WaitingOpponentState::~WaitingOpponentState()
{

}

void WaitingOpponentState::Enter(const EnterPayload& enterPayload)
{
	_pGame->GetEventMouseClick().AddListener(_delegateMouseClick);
	_pGame->GetNetworkDiscovery().StartBroadcast();

	if(!_pGame->GetNetworkGame().HostGame())
	{
		_pGame->ChangeState(GameState::Error, Error("Le réseau n'a pas pu être initialisé"));
	}
}

void WaitingOpponentState::Update()
{
	_pGame->GetWindow().draw(_titleText);
	_pGame->GetWindow().draw(_informationText);
	_pGame->GetWindow().draw(_backText);

	if(!_pGame->GetNetworkGame().IsConnectionEstablish())
	{
		if(!_pGame->GetNetworkGame().WaitingAnOpponent())
		{
			_pGame->ChangeState(GameState::Error, Error("Une erreur réseau est survenue"));
		}
	}
	else
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

void WaitingOpponentState::Exit(GameState nextState)
{
	if(nextState != GameState::Playing)
	{
		std::cout << "Game not properly initialized, Reset network" << std::endl;

		_pGame->GetNetworkGame().StopPlaying();
	}

	_pGame->GetNetworkDiscovery().StopBroadcast();
	_pGame->GetEventMouseClick().RemoveListener(_delegateMouseClick);
}

void WaitingOpponentState::HandleMouseClick(float x, float y)
{
	if(_backText.getGlobalBounds().contains(x, y))
	{
		std::cout << "Back clicked" << std::endl;

		_pGame->ChangeState(GameState::Menu);
	}
}
