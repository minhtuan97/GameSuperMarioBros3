#include "QuestionBrick.h"

#define QUESTIONBRICK_GRAVITY	0.001f

void QuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!isColi)
		return;
	CGameObject::Update(dt);
	vy += QUESTIONBRICK_GRAVITY * dt;
	y += dy;
	if (y > yde)
	{
		SetPosition(xde, yde);
		SetSpeed(0, 0);
	}
}

void QuestionBrick::Render()
{
	animation_set->at(1)->Render(x, y);
	RenderBoundingBox();
}

void QuestionBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + QUESTIONBRICK_BBOX_WIDTH;
	b = y + QUESTIONBRICK_BBOX_HEIGHT;
}
