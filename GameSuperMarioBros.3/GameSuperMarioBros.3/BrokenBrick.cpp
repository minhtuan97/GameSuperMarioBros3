#include "BrokenBrick.h"
#include "Grid.h"

BrokenBrick::BrokenBrick(float x, float y)
{
	this->x = x;
	this->y = y;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	SetAnimationSet(ani_set);
	start_broken = GetTickCount64();
}

void BrokenBrick::Render()
{
	animation_set->at(0)->Render(x, y);
}

void BrokenBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICKBROKEN_BBOX_WIDTH;
	b = y + BRICKBROKEN_BBOX_HEIGHT;
}

void BrokenBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (GetTickCount64() - start_broken > TIME_BRICK_BROKEN)
	{
		Grid::GetInstance()->deleteObject(this);
		return;
	}
	CGameObject::Update(dt);
	vy += G * dt;
	x += dx;
	y += dy;
}
