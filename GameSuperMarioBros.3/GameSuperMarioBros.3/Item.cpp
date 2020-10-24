#include "Item.h"
#include"define.h"
#include "Brick.h"
#include "Grid.h"
#include "Box.h"

#define	GOlDEN_GRAVITY	0.001f
#define TYPE_GOLDEN	0
#define TYPE_NAM	1

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
		if (y > yde)
		{
			SetPosition(xde, yde);
			SetSpeed(0, 0);
			grid->deleteObject(this);
		}
		break;
	case TYPE_NAM:
		CGameObject::Update(dt);
		//vy += GOlDEN_GRAVITY * dt;
		//y += dy;
		//x += dx;
		if (y < yde - 16)
		{
			vx = 0.05f;
			vy = 0.05;
		}
		/*if (y > yde)
		{
			SetPosition(xde, yde);
			SetSpeed(0, 0);
			grid->deleteObject(this);
		}*/
		//vector<LPCOLLISIONEVENT> coEvents;
		//vector<LPCOLLISIONEVENT> coEventsResult;

		//coEvents.clear();
		//CalcPotentialCollisions(coObjects, coEvents);
		//// No collision occured, proceed normally
		//if (coEvents.size() == 0)
		//{
		//	x += dx;
		//	y += dy;
		//}
		//else
		//{
		//	float min_tx, min_ty, nx = 0, ny;
		//	float rdx = 0;
		//	float rdy = 0;

		//	// TODO: This is a very ugly designed function!!!!
		//	FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		//	// block every object first!
		//	x += min_tx * dx + nx * 0.4f;
		//	y += min_ty * dy + ny * 0.4f;

		//	if (nx != 0) vx = 0;
		//	if (ny != 0) vy = 0;


			//
			// Collision logic with other objects
			//
			//for (UINT i = 0; i < coEventsResult.size(); i++)
			//{
			//	LPCOLLISIONEVENT e = coEventsResult[i];

			//	if (dynamic_cast<Box*>(e->obj)) // if e->obj is Goomba 
			//	{
			//		Box* box = dynamic_cast<Box*>(e->obj);

			//		// jump on top >> kill Goomba and deflect a bit 
			//		if (e->ny < 0)
			//		{
			//			if (goomba->GetState() != GOOMBA_STATE_DIE)
			//			{
			//				goomba->SetState(GOOMBA_STATE_DIE);
			//				vy = -MARIO_JUMP_DEFLECT_SPEED;
			//			}
			//		}
			//		else if (e->nx != 0)
			//		{
			//			if (untouchable == 0)
			//			{
			//				if (goomba->GetState() != GOOMBA_STATE_DIE)
			//				{
			//					if (level > MARIO_LEVEL_SMALL)
			//					{
			//						level = MARIO_LEVEL_SMALL;
			//						StartUntouchable();
			//					}
			//					else
			//						SetState(MARIO_STATE_DIE);
			//				}
			//			}
			//		}
			//	}
			//}
		//}
		// clean up collision events
		//for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
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
