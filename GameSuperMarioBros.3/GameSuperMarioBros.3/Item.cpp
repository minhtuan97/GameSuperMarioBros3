#include "Item.h"
#include"define.h"
#include "Brick.h"
#include "Grid.h"
#include "Box.h"
#include "WaterPipe.h"
#include "Utils.h"
#include "Map.h"
#include "Camera.h"

#define	GOlDEN_GRAVITY	0.001f
#define TYPE_GOLDEN	0
#define TYPE_NAM	1
#define TYPE_LA	2

Item::Item():CGameObject()
{
	isEnable = false;
	isfinish = false;
	type = 1;
	id = xde = yde = 0;
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
	case TYPE_GOLDEN:
		CGameObject::Update(dt);
		vy += GOlDEN_GRAVITY * dt;
		y += dy;
		if (y < 0) y = 0;
		if (y > yde)
		{
			SetPosition(xde, yde);
			SetSpeed(0, 0);
			grid->deleteObject(this);
		}
		break;
	case TYPE_LA:
		CGameObject::Update(dt);
		if(!isFall)
			vy += GOlDEN_GRAVITY * dt;

		y += dy;
		x += dx;
		if (y < yde)
		{
			vx = 0.5;
			isFall = true;
			vy = 0.02;
		}
		if (isFall)
		{
			if (x > xde + 24) vx = -0.1;
			if (x < xde - 24) vx = 0.1;
		}
		else
			vx = 0;
		
		if(y>Map::GetInstance()->GetHeight())
			grid->deleteObject(this);
		break;
	case TYPE_NAM:
	{
		CGameObject::Update(dt);
		if (y < yde - 16)
		{
			vx = 0.05f;
		}
		if (vx != 0)
			vy += 0.3*GOlDEN_GRAVITY * dt;
		/*if (y > yde)
		{
			SetPosition(xde, yde);
			SetSpeed(0, 0);
			grid->deleteObject(this);
		}*/
		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();
		CalcPotentialCollisions(coObjects, coEvents);
		// No collision occured, proceed normally
		if (coEvents.size() == 0)
		{
			x += dx;
			y += dy;
		}
		else
		{
			float min_tx, min_ty, nx = 0, ny;
			float rdx = 0;
			float rdy = 0;

			// TODO: This is a very ugly designed function!!!!
			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

			// block every object first!
			x += min_tx * dx + nx * 0.2f;
			y += min_ty * dy + ny * 0.2f;

			if (nx != 0) vx = 0;
			if (ny != 0) vy = 0;

			//Collision logic with other objects

			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				LPCOLLISIONEVENT e = coEventsResult[i];

				if (dynamic_cast<WaterPipe*>(e->obj))
				{
					iscoliwithpipe=true;
				}
				if (dynamic_cast<CBrick*>(e->obj))
				{
					if(iscoliwithpipe)
						vx = -0.05f;
				}
			}
		}
		// clean up collision events
		for (UINT i = 0; i < coEvents.size(); i++) 
			delete coEvents[i];
		//DebugOut(L"vx=%f, vy%f\n", vx, vy);
		break;
	}
	default:
		break;
	}
	//Grid::GetInstance()->Update(this);
	
	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;
	if (x<xcam || x>xcam + SCREEN_WIDTH || y<ycam || y>ycam + SCREEN_HEIGHT)
		Grid::GetInstance()->deleteObject(this);
	else
		Grid::GetInstance()->Update(this);
}

void Item::Render()
{
	animation_set->at(type)->Render(x, y);
	//RenderBoundingBox();
}
