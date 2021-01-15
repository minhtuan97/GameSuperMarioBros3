#pragma once
#include "GameObject.h"

#define ani_set_id	40
#define G	0.001f
#define TIME_BRICK_BROKEN	500

#define BRICKBROKEN_BBOX_WIDTH  9
#define BRICKBROKEN_BBOX_HEIGHT 9

class BrokenBrick:public CGameObject
{
public:

	BrokenBrick(float x, float y);
	DWORD start_broken;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};

