#include "Hammer.h"
#include "Grid.h"
#include "Camera.h"
#include "define.h"

Hammer::Hammer(float xstart, float ystart, int nx)
{
	x_start = xstart;
	y_start = ystart;
	this->nx = nx;
	SetPosition(xstart, ystart);
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id_hammer);
	SetAnimationSet(ani_set);
	vx = SPEED_X_HAMMER * nx;
	timecrate = GetTickCount64();
}

void Hammer::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BBOX_HAMMER_WIDTH;
	bottom = y + BBOX_HAMMER_HEIGHT;
}

void Hammer::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;
	if (GetTickCount64() - timecrate > 0 && GetTickCount64() - timecrate < TIME_SPEED_VY)
	{
		vy = -HAMMER_SPEED_Y;
	}
	else if (GetTickCount64() - timecrate > TIME_SPEED_VY_2 && GetTickCount64() - timecrate < TIME_SPEED_VY_3)
	{
		vy = HAMMER_SPEED_Y;
		if (nx > 0)
			vx = -SPEED_X_HAMMER;
		else
			vx = SPEED_X_HAMMER;

	}
	else if (GetTickCount64() - timecrate > TIME_SPEED_VY && GetTickCount64() - timecrate < TIME_SPEED_VY_2 && x < xcam + 1 && vx < 0)
	{
		if (nx > 0)
			vx = -SPEED_X_HAMMER;
		else
			vx = SPEED_X_HAMMER;
	}
	else
		vy = 0;
	CGameObject::Update(dt);
	x += dx; 
	y += dy;
	if(GetTickCount64() - timecrate>TIME_HAMMER)
		Grid::GetInstance()->deleteObject(this);
	else
		Grid::GetInstance()->Update(this);
	if (x < xcam || x>xcam + SCREEN_WIDTH - BBOX_HAMMER_WIDTH)
		Grid::GetInstance()->deleteObject(this);

}

void Hammer::Render()
{
	int ani = 0;
	animation_set->at(ani)->Render(x,y);
}
