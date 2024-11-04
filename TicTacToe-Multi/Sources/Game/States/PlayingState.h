#pragma once

#include "BaseState.h"
#include "Game/GameEvent.h"
#include "Game/Player.h"
#include "NetworkManager/GameInfo.h"

class PlayingState : public BaseState
{
public:
									PlayingState(Game* pGame);
	virtual							~PlayingState();

	virtual void					Enter(const EnterPayload& enterPayload) override;
	virtual void					Update() override;
	virtual void					Exit(GameState nextState) override;

	bool							IsLocalPlayerTurn() const;

    void                            MakeMove(int row, int col);
    Player							CheckWinner();
    bool							IsBoardFull() const;

	Player							GetCell(int row, int col) const;

private:

	void							PrintBoard();

	void							HandleMouseClick(float x, float y);

	EventMouseClick::Delegate		_delegateMouseClick;

    Player                          _localPlayer;
    Player                          _currentPlayer;
	Player							_board[BoardSize][BoardSize];
};