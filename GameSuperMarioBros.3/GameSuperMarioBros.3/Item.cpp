#include "Item.h"
#include"define.h"
#include "Brick.h"
#include "Grid.h"

#define	GOlDEN_GRAVITY	0.001f

Item::Item():CGameObject()
{
	isEnable = false;
	isfinish = false;
	type = 1;
}

Item::~Item()
{
}

void Item::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	right = x + ITEM_BIG_BBOX_WIDTH;
	top = y;
	bottom = top + ITEM_BIG_BBOX_HEIGHT;
}

void Item::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	Grid* grid = Grid::GetInstance();
	switch (type)
	{
	case 0:
		CGameObject::Update(dt);
		vy += GOlDEN_GRAVITY * dt;
		y += dy;
		if (y > yde)
		{
			SetPosition(xde, yde);
			SetSpeed(0, 0);
			grid->deleteObject(this);
		}
		break;
	default:
		break;
	}
}

void Item::Render()
{
	animation_set->at(type)->Render(x, y);
	//RenderBoundingBox();
}
