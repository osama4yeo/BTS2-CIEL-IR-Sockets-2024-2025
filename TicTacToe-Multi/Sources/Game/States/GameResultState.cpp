#include "pch.h"
#include "GameResultState.h"
#include "Game/Game.h"

GameResultState::GameResultState(Game* pGame)
	: BaseState(pGame)
	, _delegateMouseClick(std::bind_front(&GameResultState::HandleMouseClick, this))
	, _titleText("Tic-Tac-Toe en réseau", pGame->GetFont(), 40)
	, _informationText("", pGame->GetFont(), 30)
	, _backText("Retour", pGame->GetFont(), 30)
{
	_titleText.setFillColor(sf::Color::Black);
	_titleText.setPosition(WindowSize / 2 - _titleText.getLocalBounds().width / 2, 50);

	_backText.setFillColor(sf::Color::Black);
	_backText.setPosition(50, 250);
}

GameResultState::~GameResultState()
{

}

void GameResultState::Enter(const EnterPayload& enterPayload)
{
	if(!std::holds_alternative<GameResult>(enterPayload))
	{
		_pGame->ChangeState(GameState::Error, Error("Data invalide dans GameResultState::Enter"));
		return;
	}

	GameResult gameResult = std::get<GameResult>(enterPayload);

	std::cout << "Game result is " << GameResultToString(gameResult) << std::endl;

	_pGame->GetEventMouseClick().AddListener(_delegateMouseClick);

	switch(gameResult)
	{
	case GameResult::LocalPlayerWon:			_informationText.setString("Vous avez gagné !");		break;
	case GameResult::OpponentPlayerWon:			_informationText.setString("Vous avez perdu !");		break;
	default:									_informationText.setString("Match nul !");				break;
	}

	_informationText.setFillColor(sf::Color::Black);
	_informationText.setPosition(WindowSize / 2 - _informationText.getLocalBounds().width / 2, 150);
}

void GameResultState::Update()
{
	_pGame->GetWindow().draw(_titleText);
	_pGame->GetWindow().draw(_informationText);
	_pGame->GetWindow().draw(_backText);
}

void GameResultState::Exit(GameState nextState)
{
	_pGame->GetEventMouseClick().RemoveListener(_delegateMouseClick);
	_pGame->GetNetworkGame().StopPlaying();
}

void GameResultState::HandleMouseClick(float x, float y)
{
	if(_backText.getGlobalBounds().contains(x, y))
	{
		std::cout << "Back clicked" << std::endl;

		_pGame->ChangeState(GameState::Menu);
	}
}
