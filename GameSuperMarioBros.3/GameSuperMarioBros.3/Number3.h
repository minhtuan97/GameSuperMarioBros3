#pragma once
#include "GameObject.h"

#define BBOX_NUMBER3_WIDTH 42
#define BBOX_NUMBER3_HeIGHT 41

class Number3:public CGameObject
{
public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};

