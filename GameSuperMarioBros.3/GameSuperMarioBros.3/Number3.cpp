#include "Number3.h"

void Number3::Render()
{
	animation_set->at(0)->Render(x, y);
}

void Number3::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BBOX_NUMBER3_WIDTH;
	b = y + BBOX_NUMBER3_HeIGHT;
}

void Number3::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
}
