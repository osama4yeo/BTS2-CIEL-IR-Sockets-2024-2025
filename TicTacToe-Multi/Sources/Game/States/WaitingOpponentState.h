#pragma once

#include "BaseState.h"
#include "Game/GameEvent.h"
#include "NetworkManager/GameInfo.h"

class WaitingOpponentState : public BaseState
{
public:
									WaitingOpponentState(Game* pGame);
	virtual							~WaitingOpponentState();

	virtual void					Enter(const EnterPayload& enterPayload) override;
	virtual void					Update() override;
	virtual void					Exit(GameState nextState) override;

private:

	void							HandleMouseClick(float x, float y);

	EventMouseClick::Delegate		_delegateMouseClick;

	sf::Text						_titleText;
	sf::Text						_informationText;
	sf::Text						_backText;
};