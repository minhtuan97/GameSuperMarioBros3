#include "WaterPipe.h"

void WaterPipe::Render()
{
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void WaterPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + 16;
	b = y + 16;
}
