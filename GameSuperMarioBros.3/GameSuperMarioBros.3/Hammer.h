#pragma once
#include "GameObject.h"

#define BBOX_HAMMER_WIDTH	16
#define BBOX_HAMMER_HEIGHT	16
#define ani_set_id_hammer	43
#define SPEED_X_HAMMER	0.07
#define TIME_SPEED_VY	300
#define TIME_SPEED_VY_2	2300
#define TIME_SPEED_VY_3	2800
#define TIME_HAMMER	5500

#define HAMMER_SPEED_Y	0.05f
#define HAMMER_SPEED_2Y	0.1f

class Hammer:public CGameObject
{
public:
	DWORD timecrate;
	float x_start, y_start;
	Hammer(float xstart, float ystart, int nx);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
};

