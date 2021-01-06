#pragma once
#include "GameObject.h"

#define BRICKMOVE_BBOX_WIDTH  48
#define BRICKMOVE_BBOX_HEIGHT 16
#define SPEED_X -0.05f
#define SPEED_Y -0.1f

class BrickMove:public CGameObject
{
public:
	BrickMove();
	bool isColi = false;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};

