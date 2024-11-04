#include "pch.h"
#include "PlayingState.h"
#include "Game/Game.h"

PlayingState::PlayingState(Game* pGame)
	: BaseState(pGame)
	, _delegateMouseClick(std::bind_front(&PlayingState::HandleMouseClick, this))
	, _board{Player::None}
	, _localPlayer(Player::O)
	, _currentPlayer(Player::X)
{

}

PlayingState::~PlayingState()
{

}

void PlayingState::Enter(const EnterPayload& enterPayload)
{
	for(int i = 0; i < BoardSize; ++i)
	{
		for(int j = 0; j < BoardSize; ++j)
		{
			_board[i][j] = Player::None;
		}
	}

	_localPlayer = _pGame->GetNetworkGame().IsServer() ? Player::X : Player::O;
	_currentPlayer = Player::X;
	_pGame->GetEventMouseClick().AddListener(_delegateMouseClick);

	std::cout << "Start the game, Current player is " << PlayerToString(_currentPlayer) << ", Local player is " << PlayerToString(_localPlayer) << std::endl;
}

void PlayingState::Update()
{
	if(!IsLocalPlayerTurn())
	{
		int row = 0;
		int col = 0;
		sf::Socket::Status status = _pGame->GetNetworkGame().WaitingReceiveMove(row, col);

		switch(status)
		{
		case sf::Socket::Done:
		{
			MakeMove(row, col);
			break;
		}

		// No packet recv yet
		case sf::Socket::NotReady:
		{
			break;
		}

		// An error occured
		default:
		{
			_pGame->ChangeState(GameState::Error, Error("Une erreur réseau est survenue"));
			return;
		}
		}
	}

	for(int i = 1; i < BoardSize; ++i)
	{
		sf::RectangleShape line(sf::Vector2f(WindowSize, 2));
		line.setFillColor(sf::Color::Black);
		line.setPosition(0, static_cast<float>(i) * CellSize);
		_pGame->GetWindow().draw(line);

		line.setSize(sf::Vector2f(2, WindowSize));
		line.setPosition(static_cast<float>(i) * CellSize, 0);
		_pGame->GetWindow().draw(line);
	}

	// Draw X and O
	for(int row = 0; row < BoardSize; ++row)
	{
		for(int col = 0; col < BoardSize; ++col)
		{
			sf::Text text;

			if(GetCell(row, col) == Player::X)
			{
				text.setString("X");
				text.setFillColor(sf::Color::Red);
			}
			else if(GetCell(row, col) == Player::O)
			{
				text.setString("O");
				text.setFillColor(sf::Color::Blue);
			}

			text.setFont(_pGame->GetFont());
			text.setCharacterSize(static_cast<unsigned int>(CellSize) / 2);

			sf::FloatRect textBounds = text.getLocalBounds();

			text.setPosition(
				(static_cast<float>(col) * CellSize) + CellSize / 2 - (textBounds.width + textBounds.left) / 2,
				(static_cast<float>(row) * CellSize) + CellSize / 2 - (textBounds.height + textBounds.top) / 2);

			_pGame->GetWindow().draw(text);
		}
	}

	// Draw game state
	sf::Text stateText;
	stateText.setFont(_pGame->GetFont());
	stateText.setCharacterSize(24);
	stateText.setPosition(10, WindowSize + 10);
	stateText.setFillColor(sf::Color::Black);
	stateText.setString(IsLocalPlayerTurn() ? "Votre tour" : "Tour de l'adversaire");
	_pGame->GetWindow().draw(stateText);

	Player winner = CheckWinner();

	if(winner != Player::None)
	{
		std::cout << "Player " << PlayerToString(winner) << (winner == _localPlayer ? " Local" : " Opponent") << " player won" << std::endl;
		PrintBoard();

		_pGame->ChangeState(GameState::GameResult, EnterPayload(winner == _localPlayer ? GameResult::LocalPlayerWon : GameResult::OpponentPlayerWon));
	}
	else if(IsBoardFull())
	{
		std::cout << "Its a draw" << std::endl;
		PrintBoard();

		_pGame->ChangeState(GameState::GameResult, EnterPayload(GameResult::Draw));
	}
}

void PlayingState::Exit(GameState nextState)
{
	_pGame->GetEventMouseClick().RemoveListener(_delegateMouseClick);
}

bool PlayingState::IsLocalPlayerTurn() const
{
	return _currentPlayer == _localPlayer;
}

void PlayingState::MakeMove(int row, int col)
{
	if(row >= 0 && row < BoardSize && col >= 0 && col < BoardSize && _board[row][col] == Player::None)
	{
		_board[row][col] = _currentPlayer;

		std::cout << (IsLocalPlayerTurn() ? "Local player" : "Opponent player") << " play " << col << ", " << row << std::endl;

		PrintBoard();

		if(IsLocalPlayerTurn())
		{
			if(!_pGame->GetNetworkGame().SendMove(row, col))
			{
				_pGame->ChangeState(GameState::Menu);
				return;
			}
		}

		_currentPlayer = (_currentPlayer == Player::X) ? Player::O : Player::X;

		std::cout << "Next turn, Current player is " << PlayerToString(_currentPlayer) << ", Local player is " << PlayerToString(_localPlayer) << std::endl;
	}
}

Player PlayingState::CheckWinner()
{
	// Check rows and columns

	for(int i = 0; i < BoardSize; ++i)
	{
		if(_board[i][0] != Player::None && _board[i][0] == _board[i][1] && _board[i][1] == _board[i][2])
		{
			return _board[i][0];
		}
		if(_board[0][i] != Player::None && _board[0][i] == _board[1][i] && _board[1][i] == _board[2][i])
		{
			return _board[0][i];
		}
	}

	// Check diagonals

	if(_board[0][0] != Player::None && _board[0][0] == _board[1][1] && _board[1][1] == _board[2][2])
	{
		return _board[0][0];
	}

	if(_board[0][2] != Player::None && _board[0][2] == _board[1][1] && _board[1][1] == _board[2][0])
	{
		return _board[0][2];
	}

	return Player::None;
}

bool PlayingState::IsBoardFull() const
{
	for(int i = 0; i < BoardSize; ++i)
	{
		for(int j = 0; j < BoardSize; ++j)
		{
			if(_board[i][j] == Player::None)
			{
				return false;
			}
		}
	}

	return true;
}

Player PlayingState::GetCell(int row, int col) const
{
	return _board[row][col];
}

void PlayingState::PrintBoard()
{
	std::cout << "\n";

	for(int row = 0; row < BoardSize; ++row)
	{
		if (row > 0)
		{
			std::cout << "\n-----------\n";
		}

		for(int col = 0; col < BoardSize; ++col)
		{
			std::cout << " " << PlayerToString(GetCell(row, col)) << " ";

			if (col % 3 != 2)
			{
				std::cout << "|";
			}
		}
	}

	std::cout << "\n\n";
}

void PlayingState::HandleMouseClick(float x, float y)
{
	if(IsLocalPlayerTurn())
	{
		int row = static_cast<int>(y / CellSize);
		int col = static_cast<int>(x / CellSize);

		if(row < BoardSize && col < BoardSize)
		{
			MakeMove(row, col);
		}
	}
}
