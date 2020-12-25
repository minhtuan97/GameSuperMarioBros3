#include "Power.h"
#include "Sprites.h"

Power::Power()
{
}

void Power::Draw(float x, float y, int power)
{
	this->x = x;
	this->y = y;
	CSprites* sprites = CSprites::GetInstance();

	
	if (power > 5)
	{
		sprites->Get(40004)->Draw(x + 50, y);

	}
	 for (size_t i = 0; i < power&&i<6; i++)
	{
		sprites->Get(40003)->Draw(x + i * 8, y);
	}
}
