#pragma once
#include "GameObject.h"

#define HammerBros_STATE_DIE 0
#define HammerBros_STATE_WALKING 1
#define BBOX_HAMMERBROS_WIDTH	33
#define BBOX_HAMMERBROS_HEIGHT	33
#define SPEED_X_HAMMERBROS	0.02f
#define TIME_CRATE_HAMMER	3000
#define TIME_DIE	400
class HammerBros: public CGameObject
{
public:
	float x_min, x_max;
	DWORD timecrate;
	DWORD startdie;
	HammerBros();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
};

