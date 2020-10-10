#include "WaterPipe.h"

void WaterPipe::Render()
{
	RenderBoundingBox();
}

void WaterPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = right;
	b = bot;
}
