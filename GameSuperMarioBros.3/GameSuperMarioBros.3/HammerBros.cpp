#include "HammerBros.h"
#include "Mario.h"
#include "Koopas.h"
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
#include "Hammer.h"


HammerBros::HammerBros()
{
	timecrate = GetTickCount64();
	nx = 1;
	SetState(HammerBros_STATE_WALKING);
}

void HammerBros::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BBOX_HAMMERBROS_WIDTH;
	bottom = y + BBOX_HAMMERBROS_HEIGHT;
	vx = SPEED_X_HAMMERBROS;
}

void HammerBros::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (GetTickCount64() - timecrate > TIME_CRATE_HAMMER)
	{
		timecrate = GetTickCount64();
		Hammer* h = new Hammer(x, y, nx);
		Grid::GetInstance()->addObject(h);
	}
	if (GetTickCount64() - startdie > TIME_DIE && state == HammerBros_STATE_DIE)
	{
		Grid::GetInstance()->deleteObject(this);
		return;
	}
	//DebugOut(L"x=%f, y=%f\n", x, y);
	CMario* mario = CMario::GetInstance();
	float xmario = mario->x;
	float ymario = mario->y;
	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;
	if (x < xmario)
	{
		nx = 1;
		vx = SPEED_X_HAMMERBROS;
	}
	else
	{
		nx = -1;
		vx = -SPEED_X_HAMMERBROS;
	}
	if (x > x_max && nx > 0)
	{
		vx = 0;
		x = x_max;
	}
	if (x < x_min && nx < 0)
	{
		vx = 0;
		x = x_min;
	}
	CGameObject::Update(dt, coObjects);
	vy += HAMMERBROS_G * dt;
	

		
	if (x > xcam + SCREEN_WIDTH + BBOX_HAMMERBROS_WIDTH || x < xcam - BBOX_HAMMERBROS_WIDTH)
		return;
		vector<LPGAMEOBJECT> list;
		list.clear();

		for (int i = 0; i < coObjects->size(); i++)
		{
			if (dynamic_cast<CBrick*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
			if (dynamic_cast<WaterPipe*>(coObjects->at(i)))
				list.push_back(coObjects->at(i));
		}

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();
		if(state!=HammerBros_STATE_DIE)
			CalcPotentialCollisions(&list, coEvents);

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
			x += min_tx * dx + nx * 0.4f;
			y += min_ty * dy + ny * 0.2f;

			if (nx != 0) vx = 0;
			if (ny != 0) vy = 0;

		}
		// clean up collision events
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
		
		

	if (x > xcam && x<xcam + SCREEN_WIDTH - BBOX_HAMMERBROS_WIDTH && y>ycam && y < ycam + SCREEN_HEIGHT - BBOX_HAMMERBROS_HEIGHT)
		Grid::GetInstance()->Update(this);
	////xoa khỏi khi ra khoi map theo trục y
	else if (this->y<ycam || this->y>ycam + SCREEN_HEIGHT - BBOX_HAMMERBROS_HEIGHT)
		Grid::GetInstance()->deleteObject(this);
}

void HammerBros::Render()
{
	int ani = 0;
	if (nx > 0)
		ani = 1;
	else if (nx < 0)
		ani = 0;
	animation_set->at(ani)->Render(x,y);
	RenderBoundingBox();
}

void HammerBros::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case HammerBros_STATE_DIE:
		vx = 0;
		//vy = 0;
		startdie = GetTickCount64();
		break;
	case HammerBros_STATE_WALKING:
		vx = SPEED_X_HAMMERBROS *nx;
		break;
	}
}
