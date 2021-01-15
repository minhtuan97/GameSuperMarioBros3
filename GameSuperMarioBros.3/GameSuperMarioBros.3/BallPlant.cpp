#include "BallPlant.h"
#include "Mario.h"
#include "Grid.h"
#include "Camera.h"
#include "define.h"

#define BALLPLANT_BBOX_WIDTH  8
#define BALLPLANT_BBOX_HEIGHT 8
#define ANIMATIONSET 5
#define BALLPLANT_SPEED_Y 0.05
#define BALLPLANT_SPEED_Y2 0.03


void BallPlant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + BALLPLANT_BBOX_WIDTH;
	bottom = top + BALLPLANT_BBOX_HEIGHT;
}

void BallPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CGameObject::Update(dt);
	x += dx;
	y += dy;
	
	

	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;

	if (x > xcam && x<xcam + SCREEN_WIDTH && y>ycam && y < ycam + SCREEN_HEIGHT-16)
		Grid::GetInstance()->Update(this);
	else
		Grid::GetInstance()->deleteObject(this);

	//if (isattack)
	//	Grid::GetInstance()->deleteObject(this);

}

void BallPlant::Render()
{
	int ani = 0;
	animation_set->at(ani)->Render(x, y);
}

BallPlant::BallPlant(float xx, float yy, int nx, int ny)
{
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(ANIMATIONSET);
	SetAnimationSet(ani_set);


	int xstart = (int)xx;
	int ystart = (int)yy;
	SetPosition(xstart, ystart);

	

	CMario* mario = CMario::GetInstance();
	float xmario, ymario;
	mario->GetPosition(xmario, ymario);
	int xm = (int)xmario;
	int ym = (int)ymario;

	if(nx<0)
		if (ny < 0)
		{
			if (ym - y > x - xm)
			{
				vy = BALLPLANT_SPEED_Y;
				vx = -vy;
			}
			else
			{
				vy = BALLPLANT_SPEED_Y;
				vx = -2 * vy;
			}
		}
		else
		{
			if (y - ym > x - xm)
			{
				vy = -BALLPLANT_SPEED_Y;
				vx = vy;
			}
			else
			{
				vy = -BALLPLANT_SPEED_Y;
				vx = 2 * vy;
			}
		}
	else
		if (ny < 0)
		{
			if (y-ym<xm-x)
			{
				vy = BALLPLANT_SPEED_Y;
				vx = vy;
			}
			else
			{
				vy = BALLPLANT_SPEED_Y;
				vx = 2 * vy;
			}
		}
		else
		{
			if (y - ym <xm-x)
			{
				vy = -BALLPLANT_SPEED_Y2;
				vx = -vy;
			}
			else
			{
				vy = -BALLPLANT_SPEED_Y2;
				vx = -2 * vy;
			}
		}
}
