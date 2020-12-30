#include "ItemSelect.h"
#include "Utils.h"
#include "Camera.h"
#include "Grid.h"
#include "Mario.h"

void ItemSelect::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + 16;
	bottom = y + 16;
}

void ItemSelect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	x += dx;
	y += dy;

	if (GetTickCount64() - ani_start > RENDER_TIME&&!isColi)
	{
		ani_start = GetTickCount64();
		type++;
		if (type > 2)
			type = 0;
	}
	if (isColi)
		vy = -0.1f;

	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;
	if (y<ycam)
	{
		CMario::GetInstance()->iscard = true;
		CMario::GetInstance()->typecard = type;
		Grid::GetInstance()->deleteObject(this);
	}
}

void ItemSelect::Render()
{
	animation_set->at(type)->Render(x, y);
	DebugOut(L"type:%d\n", type);
}

ItemSelect::ItemSelect()
{
	vx = vy = 0;
}

int ItemSelect::Get_animationset_at_k()
{
	return type;
}
