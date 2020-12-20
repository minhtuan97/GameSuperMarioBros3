#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16
#define CLOUD	1
#define BRICK2	2
#define BRICK3	3

class CBrick : public CGameObject
{

public:
	int type;
	int right, bot;
	virtual void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};