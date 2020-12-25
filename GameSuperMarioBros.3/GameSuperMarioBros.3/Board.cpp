#include "Board.h"

string Board::FillNumber(string s, UINT MaxNumber)
{
	while (s.size() < MaxNumber)
		s = "0" + s;
	return s;
}

Board::Board()
{
	this->x = 0;
	this->y = 0;
}

Board::Board(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Board::SetPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Board::Render(CMario* mario, int time)
{
	CSprites* sprites = CSprites::GetInstance();

	sprites->Get(40001)->Draw(x, y);
	sprites->Get(40002)->Draw(x+160, y);

	//_font.Draw(x + 100, y + 15, FillNumber(std::to_string(simon->GetScore()), 6));
	coin.Draw(x + 132, y + 6, FillNumber(std::to_string(mario->money), 2));
	power.Draw(x + 52, y+6,(int) (mario->power/10));
	life.Draw(x + 29, y + 15, FillNumber(std::to_string(mario->m), 2));
	timer.Draw(x + 124, y + 15, FillNumber(std::to_string(time), 3));
	enemy.Draw(x + 52, y + 15, FillNumber(std::to_string(mario->enymy), 7));
	scence.Draw(x + 37, y + 8, FillNumber(std::to_string(mario->scence), 1));

}

Board::~Board()
{
}
