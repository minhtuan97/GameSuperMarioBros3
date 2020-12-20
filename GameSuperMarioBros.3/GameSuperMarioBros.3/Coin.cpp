#include "Coin.h"

void Coin::Render()
{
	animation_set->at(0)->Render(x, y);
}

void Coin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x+COIN_BBOX_WIDTH;
	b = y+COIN_BBOX_HEIGHT;
}
