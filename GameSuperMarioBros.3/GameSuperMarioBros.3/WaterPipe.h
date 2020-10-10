#pragma once
#include "GameObject.h"

class WaterPipe: public CGameObject
{
public:
	int type = 0;
	int right, bot;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

