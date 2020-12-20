#include "BallPlant.h"
#include "Mario.h"
#include "Grid.h"
#include "Camera.h"
#include "define.h"

void BallPlant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + 8;
	bottom = top + 8;
}

void BallPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CGameObject::Update(dt);
	x += dx;
	y += dy;
	
	if(isattack)
		Grid::GetInstance()->deleteObject(this);

	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;

	//if (x > xcam && x<xcam + SCREEN_WIDTH && y>ycam && y < ycam + SCREEN_HEIGHT)
	//	//	Grid::GetInstance()->Update(this);
	//	;
	//else
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
	LPANIMATION_SET ani_set = animation_sets->Get(5);
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
				vy = 0.05;
				vx = -vy;
			}
			else
			{
				vy = 0.05;
				vx = -2 * vy;
			}
		}
		else
		{
			if (y - ym > x - xm)
			{
				vy = -0.05;
				vx = vy;
			}
			else
			{
				vy = -0.05;
				vx = 2 * vy;
			}
		}
	else
		if (ny < 0)
		{
			if (y-ym<xm-x)
			{
				vy = 0.05;
				vx = vy;
			}
			else
			{
				vy = 0.05;
				vx = 2 * vy;
			}
		}
		else
		{
			if (y - ym <xm-x)
			{
				vy = -0.03;
				vx = -vy;
			}
			else
			{
				vy = -0.03;
				vx = -2 * vy;
			}
		}
}
