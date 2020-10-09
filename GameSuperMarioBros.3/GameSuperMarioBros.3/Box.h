#pragma once
#include "GameObject.h"

class Box:public CGameObject
{

public:
		int right, bot;
		virtual void Render();
		virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

