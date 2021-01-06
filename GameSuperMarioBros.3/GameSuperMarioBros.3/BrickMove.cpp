#include "BrickMove.h"
#include "Utils.h"
#include "Camera.h"
#include "define.h"
#include "Grid.h"

BrickMove::BrickMove()
{
	vx = vy = 0;
}

void BrickMove::Render()
{
	animation_set->at(0)->Render(x,y);
	RenderBoundingBox();
}

void BrickMove::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICKMOVE_BBOX_WIDTH;
	b = y + BRICKMOVE_BBOX_HEIGHT;
}

void BrickMove::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	float xcam, ycam;
	xcam = Camera::GetInstance()->GetCameraPosition().x;
	ycam = Camera::GetInstance()->GetCameraPosition().y;
	CGameObject::Update(dt);
	x += dx;
	y += dy;
	if (x < xcam + SCREEN_HEIGHT)
	{
		//DebugOut(L"Update brick move\n");
		if (!isColi)
		{
			vx = SPEED_X;
			vy = 0;
		}
		else
		{
			vx = 0;
			vy = -SPEED_Y;
		}
		Grid::GetInstance()->Update(this);
	}
	if (x < 0||y>ycam+SCREEN_HEIGHT-BRICKMOVE_BBOX_HEIGHT- BROAD_HEIGHT)
	{
		Grid::GetInstance()->deleteObject(this);
		return;
	}
}
