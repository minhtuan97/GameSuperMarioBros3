#include "ScenceSelect.h"

ScenceSelect::ScenceSelect()
{
}

void ScenceSelect::Render()
{
	RenderBoundingBox();
}

void ScenceSelect::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + 16;
	b = y + 16;
}
