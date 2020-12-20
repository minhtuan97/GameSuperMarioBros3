#include "Brick.h"

void CBrick::Render()
{
	if(type	== CLOUD)
		animation_set->at(0)->Render(x, y);
	if (type == BRICK2)
		animation_set->at(1)->Render(x, y);
	if (type == BRICK3)
		animation_set->at(2)->Render(x, y);
	RenderBoundingBox();
}

void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x;
	t = y;
	if (type == BRICK2||type==BRICK3)
	{
		r = x + 16;
		b = y + 16;
	}
	else
	{
		r = right;
		b = bot;
	}
}