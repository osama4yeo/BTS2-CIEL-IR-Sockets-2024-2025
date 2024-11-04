#pragma once

#include "BaseState.h"
#include "Game/GameEvent.h"
#include "NetworkManager/GameInfo.h"

class ErrorState : public BaseState
{
public:
									ErrorState(Game* pGame);
	virtual							~ErrorState();

	virtual void					Enter(const EnterPayload& enterPayload) override;
	virtual void					Update() override;
	virtual void					Exit(GameState nextState) override;

private:

	void							HandleMouseClick(float x, float y);

	EventMouseClick::Delegate		_delegateMouseClick;

	sf::Text						_titleText;
	sf::Text						_informationText;
	sf::Text						_backText;

	Error							_error;
};