#include "Plant.h"
#include "Mario.h"
#include "BallPlant.h"
#include "Grid.h"
#include "Camera.h"
#include "define.h"

void Plant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BRICK_PLANT_WIDTH;
	bottom = y + BRICK_PLANT_HEIGHT;
}

void Plant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	x += dx;
	y += dy;
	if (state == PLANT_STATE_UP)
	{
		if (y < y_min)
			SetState(PLANT_STATE_ATTACK);
	}
	if (state == PLANT_STATE_DOWN)
	{
		if (y > y_max)
			SetState(PLANT_STATE_WAIT);
	}
	if (state == PLANT_STATE_ATTACK&&GetTickCount64()-attack_start>PLANT_ATTACK_TIME)
	{
		SetState(PLANT_STATE_DOWN);
		attack_start = 0;
	}
	if (state == PLANT_STATE_WAIT && GetTickCount64() - wait_start > PLANT_WAIT_TIME)
	{
		SetState(PLANT_STATE_UP);
		wait_start = 0;
	}

	//update nx, ny
	CMario* mario = CMario::GetInstance();
	float xm, ym;
	mario->GetPosition(xm, ym);
	if (ym > y_min)
		ny = -1;
	else
		ny = 1;
	if (xm < this->x)
		nx = -1;
	else
		nx = 1;
	if (state == PLANT_STATE_ATTACK && !isattack&&type!=1)
	{
		BallPlant* b = new BallPlant(x, y, nx, ny);
		Grid* grid = Grid::GetInstance();
		grid->addObject(b);
		isattack = true;
	}

	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;

	if (x > xcam && x<xcam + SCREEN_WIDTH - 16 && y>ycam && y < ycam + SCREEN_HEIGHT - 32)
		Grid::GetInstance()->Update(this);
}

void Plant::Render()
{
	int ani = 0;
	switch (type)
	{
	case PLANT_TYPE0:
	{
		if (nx < 0)
		{
			if (ny > 0)
				ani = 5;
			else
				ani = 4;
		}
		else
		{
			if (ny > 0)
				ani = 9;
			else
				ani = 8;
		}
	}
		break;
	case PLANT_TYPE1:
		ani = 0;
		break;
	case PLANT_TYPE2:
		if (nx < 0)
		{
			if (ny > 0)
				ani = 2;
			else
				ani = 1;
		}
		else
		{
			if (ny > 0)
				ani = 7;
			else
				ani = 6;
		}
		break;
	default:
		break;
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

Plant::Plant(int type, float x, float y)
{
	this->type = type;
	SetPosition(x, y);
	y_min = y_max = 0;
	SetState(PLANT_STATE_UP);
}

void Plant::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case PLANT_STATE_WAIT:
		vx = 0;
		vy = 0;
		wait_start = GetTickCount64();
		break;
	case PLANT_STATE_UP:
		vx = 0;
		vy = -PLANT_SPEED_Y;
		break;
	case PLANT_STATE_DOWN:
		vx = 0;
		vy = PLANT_SPEED_Y;
		isattack = false;
		break;
	case PLANT_STATE_ATTACK:
		vx = 0;
		vy = 0;
		attack_start = GetTickCount64();
		break;
	}
}
