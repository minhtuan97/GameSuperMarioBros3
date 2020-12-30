#pragma once
#include "Font.h"
#include "Mario.h"
#include "Power.h"

#define BOARD_DEFAULT_POSITION_X 0.0f
#define BOARD_DEFAULT_POSITION_Y 0.0f

class Board
{
private:
	string FillNumber(string s, UINT MaxNumber);

public:
	Font word;
	Font coin;
	Font enemy;
	Font timer;
	Font life;
	Power power;
	Font scence;
	

	float x, y;

	Board();
	Board(float X, float Y);
	void SetPosition(float x, float y);
	void Render(CMario* mario, int time);
	~Board();
};

