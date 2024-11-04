#pragma once

#include "BaseState.h"
#include "Game/GameEvent.h"
#include "NetworkManager/GameInfo.h"
#include "NetworkManager/NetworkDiscovery.h"

class MenuState : public BaseState
{
public:
									MenuState(Game* pGame);
	virtual							~MenuState();

	virtual void					Enter(const EnterPayload& enterPayload) override;
	virtual void					Update() override;
	virtual void					Exit(GameState nextState) override;

private:

	void							HandleMouseClick(float x, float y);
	void							HandleDiscoverServer(const GameInfo& gameInfo);
	void							HandleUndiscoverServer(const GameInfo& gameInfo);

	using DelegateDiscoverServer	= NetworkDiscovery::EventDiscoverServer::Delegate;
	using DelegateUndiscoverServer	= NetworkDiscovery::EventUndiscoverServer::Delegate;

	EventMouseClick::Delegate		_delegateMouseClick;
	DelegateDiscoverServer			_delegateDiscoverServer;
	DelegateUndiscoverServer		_delegateUndiscoverServer;

	sf::Text						_titleText;
	sf::Text						_informationText;
	sf::Text						_backText;

	struct AvailableGames
	{
		sf::Text					_gameText;
		GameInfo					_gameInfo;
	};

	std::vector<AvailableGames>		_availableGames;
};