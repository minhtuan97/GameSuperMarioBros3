﻿#include "Koopas.h"
#include "Grid.h"
#include "Game.h"
#include "Box.h"
#include "Brick.h"
#include "QuestionBrick.h"
#include "WaterPipe.h"
#include "Mario.h"
#include "Utils.h"
#include "define.h"
#include "BrokenBrick.h"

CKoopas::CKoopas(int t, float x, float y)
{
	SetPosition(x, y);	
	if (type == 0)
	{
		this->type = t;
		SetState(KOOPAS_STATE_WALKING);
	}
	if (type == 1)
	{
		this->type = t;
		SetState(KOOPAS_STATE_FLY);
	}
	if (type == 2)
	{
		this->type = KOOPAS_TYPE_2;
		SetState(KOOPAS_STATE_WALKING);
	}
	if (t == 3)
	{
		this->type = KOOPAS_TYPE_3;
		SetState(KOOPAS_STATE_WALKING);
		SetSpeed(0, KOOPAS_SPEED_Y);
	}
}

void CKoopas::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;

	if (state == KOOPAS_STATE_DIE||state==KOOPAS_STATE_KICK)
		bottom = y + KOOPAS_BBOX_HEIGHT_DIE;
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CMario* mario = CMario::GetInstance();
	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;
	if (x > xcam + SCREEN_WIDTH+KOOPAS_BBOX_WIDTH || x < xcam-KOOPAS_BBOX_WIDTH)
		return;
	switch (type)
	{
	case KOOPAS_TYPE_1:
	{
		if (ishold)
		{
			float xm, ym;
			mario->GetPosition(xm, ym);
			if (mario->GetLevel() == MARIO_LEVEL_SMALL)
			{
				if (mario->nx > 0)
					SetPosition(xm + 16, ym);
				else if (mario->nx < 0)
					SetPosition(xm - 16, ym);
			}
			else
			{
				if (mario->nx > 0)
					SetPosition(xm + 16, ym + 12);
				else if (mario->nx < 0)
					SetPosition(xm - 16, ym + 12);
			}
			Grid::GetInstance()->Update(this);
			return;
		}
		CGameObject::Update(dt, coObjects);
		vy += KOOPAS_G * dt;

		vector<LPGAMEOBJECT> list;
		list.clear();

		for (int i = 0; i < coObjects->size(); i++)
		{
			if (dynamic_cast<Box*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<CBrick*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<WaterPipe*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<QuestionBrick*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
		}

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();
		CalcPotentialCollisions(&list, coEvents);

		// No collision occured, proceed normally
			//DebugOut(L"coewvnt: %d\n", coEvents.size());
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

			// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
			//if (rdx != 0 && rdx!=dx)
			//	x += nx*abs(rdx); 

			// block every object first!
			x += min_tx * dx + nx * 0.4f;
			y += min_ty * dy + ny * 0.4f;

			//DebugOut(L"nx=%d, vx=%f\n", nx, vx);

			if (nx != 0) vx = 0;
			if (ny != 0) vy = 0;
			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				LPCOLLISIONEVENT e = coEventsResult[i];
				if (dynamic_cast<WaterPipe*>(e->obj)  || dynamic_cast<Box*>(e->obj))
				{
					if (nx > 0)
					{
						vx = KOOPAS_KICK_SPEED;
					}
					else if (nx < 0)
					{
						vx = -KOOPAS_KICK_SPEED;
					}
					//DebugOut(L"nx=%d, vx=%f\n", nx, vx);			
				}
				if(dynamic_cast<CBrick*>(e->obj))
				{
					CBrick* b = dynamic_cast<CBrick*>(e->obj);
					if (nx != 0)
					{
						if (state== KOOPAS_STATE_KICK)
						{
							if (nx > 0)
							{
								vx = KOOPAS_KICK_SPEED;
							}
							else if (nx < 0)
							{
								vx = -KOOPAS_KICK_SPEED;
							}
						}
						if(state!= KOOPAS_STATE_WALKING)
							if (b->type == BRICK3)
						{
							b->isColi = true;
							float left, top, right, bot;
							b->GetBoundingBox(left, top, right, bot);
							float xnew = left + (right - left) / 2;
							float ynew = top + (bot - top) / 2;
							BrokenBrick* broken1 = new BrokenBrick(xnew, ynew);
							broken1->SetSpeed(-SPEED_BRICK_X, -SPEED_BRICK_YY);
							BrokenBrick* broken2 = new BrokenBrick(xnew, ynew);
							broken2->SetSpeed(SPEED_BRICK_X, -SPEED_BRICK_YY);
							BrokenBrick* broken3 = new BrokenBrick(xnew, ynew);
							broken3->SetSpeed(-SPEED_BRICK_X, -SPEED_BRICK_Y);
							BrokenBrick* broken4 = new BrokenBrick(xnew, ynew);
							broken4->SetSpeed(SPEED_BRICK_X, -SPEED_BRICK_Y);
							Grid::GetInstance()->addObject(broken1);
							Grid::GetInstance()->addObject(broken2);
							Grid::GetInstance()->addObject(broken3);
							Grid::GetInstance()->addObject(broken4);
						}
					}
			
				}
				if (dynamic_cast<QuestionBrick*>(e->obj))
				{
					QuestionBrick* brick = dynamic_cast<QuestionBrick*>(e->obj);
					if (e->nx != 0)
					{
						if (!brick->isColi)
						{
							brick->isColi = true;
							brick->SetSpeed(0, -0.1);
						}
						SetState(KOOPAS_STATE_DIE);
						if (nx > 0)
						{
							vx = KOOPAS_KICK_SPEED;
						}
						else if (nx < 0)
						{
							vx = -KOOPAS_KICK_SPEED;
						}
					}
					if (e->ny>0)
					{
						DebugOut(L"Va cham tren\n");
					}
					else if(e->ny<0)
					{
						//DebugOut(L"vy=%f\n",brick->vy);
						//SetState(KOOPAS_STATE_DIE);
						/*if(vy>0)
							DebugOut(L"vy>0\n");
						else if(vy<0)
							DebugOut(L"vy<0\n");*/

					}
					//DebugOut(L"nx=%d, vx=%f\n", nx, vx);			
				}
			}

		}
		// clean up collision events
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

		if (state == KOOPAS_STATE_WALKING)
		{
			if (x >= x_max - KOOPAS_BBOX_WIDTH)
			{
				vx = -KOOPAS_WALKING_SPEED;
				x = (float)(x_max - KOOPAS_BBOX_WIDTH);
			}
			if (x < x_min)
			{
				vx = KOOPAS_WALKING_SPEED;
				x = x_min;
			}
		}
		break;
	}
	case KOOPAS_TYPE_2:
	{
		if (ishold)
		{
			float xm, ym;
			mario->GetPosition(xm, ym);
			if (mario->GetLevel() == MARIO_LEVEL_SMALL)
			{
				if (mario->nx > 0)
					SetPosition(xm + 16, ym);
				else if (mario->nx < 0)
					SetPosition(xm - 16, ym);
			}
			else
			{
				if (mario->nx > 0)
					SetPosition(xm + 16, ym + 12);
				else if (mario->nx < 0)
					SetPosition(xm - 16, ym + 12);
			}
			Grid::GetInstance()->Update(this);
			return;
		}
		CGameObject::Update(dt, coObjects);
		vy += 0.0008f * dt;

		vector<LPGAMEOBJECT> list;
		list.clear();

		for (int i = 0; i < coObjects->size(); i++)
		{
			if (dynamic_cast<Box*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<CBrick*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<WaterPipe*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<QuestionBrick*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
		}

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();
		CalcPotentialCollisions(&list, coEvents);

		// No collision occured, proceed normally
			//DebugOut(L"coewvnt: %d\n", coEvents.size());
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

			// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
			//if (rdx != 0 && rdx!=dx)
			//	x += nx*abs(rdx); 

			// block every object first!
			x += min_tx * dx + nx * 0.2f;
			y += min_ty * dy + ny * 0.2f;

			//DebugOut(L"nx=%d, vx=%f\n", nx, vx);

			if (nx != 0) vx = 0;
			if (ny != 0) vy = 0;
			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				LPCOLLISIONEVENT e = coEventsResult[i];
				if (dynamic_cast<WaterPipe*>(e->obj) || dynamic_cast<CBrick*>(e->obj) || dynamic_cast<Box*>(e->obj))
				{
					/*if (nx > 0)
					{
						vx = KOOPAS_KICK_SPEED;
					}
					else if (nx < 0)
					{
						vx = -KOOPAS_KICK_SPEED;
					}
					else*/ 
						//SetState(KOOPAS_STATE_FLY);
					//DebugOut(L"nx=%d, vx=%f\n", nx, vx);	
					if (state == KOOPAS_STATE_FLY)
						if (ny < 0)
							vy = KOOPAS_FLY_SPEED;
				}
				if (dynamic_cast<QuestionBrick*>(e->obj))
				{
					QuestionBrick* brick = dynamic_cast<QuestionBrick*>(e->obj);

					if (nx > 0)
					{
						vx = KOOPAS_KICK_SPEED;
					}
					else if (nx < 0)
					{
						vx = -KOOPAS_KICK_SPEED;
					}
					if (e->nx != 0)
					{
						if (!brick->isColi)
						{
							brick->isColi = true;
							brick->SetSpeed(0, -0.1);
						}
					}
					//DebugOut(L"nx=%d, vx=%f\n", nx, vx);			
				}
			}

		}
		// clean up collision events
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

		/*if (state == KOOPAS_STATE_WALKING)
		{
			if (x >= x_max - KOOPAS_BBOX_WIDTH)
			{
				vx = -KOOPAS_WALKING_SPEED;
				x = (float)(x_max - KOOPAS_BBOX_WIDTH);
			}
			if (x < x_min)
			{
				vx = KOOPAS_WALKING_SPEED;
				x = x_min;
			}
		}*/
		break;
	}
	case KOOPAS_TYPE_3:
		CGameObject::Update(dt);
		y += dy;
		
		if (state != KOOPAS_STATE_DIE)
		{
			if (y < x_min)
				vy = KOOPAS_SPEED_Y;
			if (y > x_max)
				vy = -KOOPAS_SPEED_Y;
		}
		break;
	default:
		break;
	}
	
	//DebugOut(L"koopas x=%f, y=%f\n", x, y);



	if (x > xcam && x<xcam + SCREEN_WIDTH - KOOPAS_BBOX_WIDTH&& y>ycam && y < ycam + SCREEN_HEIGHT-KOOPAS_BBOX_HEIGHT)
		Grid::GetInstance()->Update(this);

	////xoa khỏi khi ra khoi map theo trục y
	//else if (this->y<ycam || this->y>ycam + SCREEN_HEIGHT - KOOPAS_BBOX_HEIGHT)
	if (this->y<ycam || this->y>ycam + SCREEN_HEIGHT+32)
		Grid::GetInstance()->deleteObject(this);
}

void CKoopas::Render()
{
	int ani = KOOPAS_ANI_WALKING_LEFT;
	switch (type)
	{
	case KOOPAS_TYPE_1:
	{
		if (state == KOOPAS_STATE_DIE)
		{
			ani = KOOPAS_ANI_DIE;
		}
		else
		{
			if (vx > 0) ani = KOOPAS_ANI_WALKING_RIGHT;
			else
				if (vx <= 0) ani = KOOPAS_ANI_WALKING_LEFT;
		}
		if (state == KOOPAS_STATE_KICK)
		{
			ani = KOOPAS_ANI_KICK;
		}
		if (ishold)
		{
			ani = KOOPAS_ANI_HOLD;
		}
		break;
	}
	case KOOPAS_TYPE_2:
	{
		if (state == KOOPAS_STATE_DIE)
		{
			ani = KOOPAS_GREEN_ANI_DIE;
		}
		else
		{
			if (state == KOOPAS_STATE_FLY)
			{
				if (vx > 0)
					ani = KOOPAS_GREEN_ANI_FLY_RIGHT;
				else if (vx <= 0) ani = KOOPAS_GREEN_ANI_FLY_LEFT;
			}
			else if (vx > 0) 
				ani = KOOPAS_GREEN_ANI_WALKING_RIGHT;
				else if (vx <= 0) ani = KOOPAS_GREEN_ANI_WALKING_LEFT;
		}
		if (state == KOOPAS_STATE_KICK)
		{
			ani = KOOPAS_GREEN_ANI_KICK;
		}
		if (ishold)
		{
			ani = KOOPAS_GREEN_ANI_HOLD;
		}
		break;
	}
	case KOOPAS_TYPE_3:
		if (state == KOOPAS_STATE_DIE)
		{
			ani = KOOPAS_ANI_DIE;
		}
		else
		{
			ani = KOOPAS_ANI_FLY;
		}
		break;
	default:
		break;
	}

	animation_set->at(ani)->Render(x, y);

	RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPAS_STATE_DIE:
		//y += KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		if (type == KOOPAS_TYPE_3)
			vy = KOOPAS_SPEED_Y_DIE;
		break;
	case KOOPAS_STATE_WALKING:
	{
		float xmario, ymario;
		CMario::GetInstance()->GetPosition(xmario, ymario);
		if (x > xmario)
			vx = -KOOPAS_WALKING_SPEED
		else
			vx = KOOPAS_WALKING_SPEED;

		break;
	}
	case KOOPAS_STATE_KICK:
	{
		int n = 0;
		float xmario, ymario;
		CMario::GetInstance()->GetPosition(xmario, ymario);
		float l, t, r, b;
		GetBoundingBox(l, t, r, b);
		if (CMario::GetInstance()->isJump)
		{
			if (xmario >= (l + r) / 2)
				n = -1;
			else
				n = 1;
			CMario::GetInstance()->nx = n;

		}
		else
		{
			n = CMario::GetInstance()->nx;
		}
		//nx = n;
		vx = n * KOOPAS_KICK_SPEED;
		break;
	}
	case KOOPAS_STATE_FLY:
	{
		float xmario, ymario;
		CMario::GetInstance()->GetPosition(xmario, ymario);
		float l, t, r, b;
		GetBoundingBox(l, t, r, b);
		if (this->x >= xmario)
		{
			vx = -1.5*KOOPAS_WALKING_SPEED;
		}
		else
		{
			vx = 1.5*KOOPAS_WALKING_SPEED;
		}
		vy = KOOPAS_FLY_SPEED;
		break;
	}
	
}

}