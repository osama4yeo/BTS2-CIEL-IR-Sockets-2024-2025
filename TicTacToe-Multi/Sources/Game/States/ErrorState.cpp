#include "pch.h"
#include "ErrorState.h"
#include "Game/Game.h"

ErrorState::ErrorState(Game* pGame)
	: BaseState(pGame)
	, _delegateMouseClick(std::bind_front(&ErrorState::HandleMouseClick, this))
	, _titleText("Tic-Tac-Toe en réseau", pGame->GetFont(), 40)
	, _informationText("En attente d'un joueur...", pGame->GetFont(), 30)
	, _backText("Retour", pGame->GetFont(), 30)
	, _error("")
{
	_titleText.setFillColor(sf::Color::Black);
	_titleText.setPosition(WindowSize / 2 - _titleText.getLocalBounds().width / 2, 50);

	_informationText.setFillColor(sf::Color::Red);
}

ErrorState::~ErrorState()
{

}

void ErrorState::Enter(const EnterPayload& enterPayload)
{
	_pGame->GetNetworkGame().StopPlaying();

	if(!std::holds_alternative<Error>(enterPayload))
	{
		exit(1);
		return;
	}

	_error = std::get<Error>(enterPayload);

	std::cerr << (_error.isFatal ? "[FATAL ERROR] " : "") << _error.error.toUtf8().c_str() << std::endl;

	_informationText.setString(_error.error);
	_informationText.setPosition(WindowSize / 2 - _informationText.getLocalBounds().width / 2, 150);
	_backText.setString(_error.isFatal ? "Quitter" : "Retour");
	_backText.setFillColor(sf::Color::Black);
	_backText.setPosition(50, 250);

	_pGame->GetEventMouseClick().AddListener(_delegateMouseClick);
}

void ErrorState::Update()
{
	_pGame->GetWindow().draw(_titleText);
	_pGame->GetWindow().draw(_informationText);
	_pGame->GetWindow().draw(_backText);
}

void ErrorState::Exit(GameState nextState)
{
	_pGame->GetEventMouseClick().RemoveListener(_delegateMouseClick);
}

void ErrorState::HandleMouseClick(float x, float y)
{
	if(_backText.getGlobalBounds().contains(x, y))
	{
		if(_error.isFatal)
		{
			exit(1);
		}
		else
		{
			_pGame->ChangeState(GameState::Menu);
		}
	}
}
