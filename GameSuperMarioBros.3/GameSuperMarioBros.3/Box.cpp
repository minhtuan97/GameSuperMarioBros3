#include "Box.h"

void Box::Render()
{
	RenderBoundingBox();
}

void Box::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = right;
	b = bot;
}
