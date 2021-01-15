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
#define	NAM_GRAVITY	0.003f
#define TYPE_GOLDEN	0
#define TYPE_NAM	1
#define TYPE_LA	2

#define LA_SPEED_X 0.5f
#define LA_SPEED_y 0.02f
#define LA_SPEED_FALL_X 0.1f
#define NAM_SPEED_X 0.05f

#define HEIGHT_LA 24
#define HEIGHT_NAM 16

Item::Item():CGameObject()
{
	isEnable = false;
	isfinish = false;
	type = TYPE_NAM;
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
			vx = LA_SPEED_X;
			isFall = true;
			vy = LA_SPEED_y;
		}
		if (isFall)
		{
			if (x > xde + HEIGHT_LA) vx = -LA_SPEED_FALL_X;
			if (x < xde - HEIGHT_LA) vx = LA_SPEED_FALL_X;
		}
		else
			vx = 0;
		
		if(y>Map::GetInstance()->GetHeight())
			grid->deleteObject(this);
		break;
	case TYPE_NAM:
	{
		CGameObject::Update(dt);
		if (y < yde - HEIGHT_NAM)
		{
			vx = NAM_SPEED_X;
		}
		if (vx != 0)
			vy += NAM_GRAVITY * dt;
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
						vx = -NAM_SPEED_X;
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
