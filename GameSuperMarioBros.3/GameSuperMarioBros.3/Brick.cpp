#include "Brick.h"

CBrick::CBrick(float x, float y)
{
	x_de = x;
	y_de = y;
	SetSpeed(0, 0);
}

void CBrick::Render()
{
	if(type	== CLOUD)
		animation_set->at(0)->Render(x, y);
	if (type == BRICK2)
		animation_set->at(1)->Render(x, y);
	if (type == BRICK3)
		animation_set->at(2)->Render(x, y);
	if (type == BRICK4)
		animation_set->at(3)->Render(x, y);
	if (type == BRICK5)
	{
		if(isP)
			animation_set->at(4)->Render(x, y);
		else
		animation_set->at(2)->Render(x, y);
	}
	RenderBoundingBox();
}

void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x;
	t = y;
	if (type == BRICK2||type==BRICK3||type==BRICK4||type==BRICK5)
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

void CBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!isP)
		return;
	CGameObject::Update(dt);
	vy += BRICK_GRAVITY * dt;
	y += dy;
	if (y > y_de)
	{
		SetPosition(x_de, y_de);
		SetSpeed(0, 0);
	}
}
