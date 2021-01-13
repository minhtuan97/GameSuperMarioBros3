#pragma once
#include "GameObject.h"
#define BBOX_SELECTPLAYER_WIDTH 112
#define BBOX_SELECTPLAYER_HEIGHT 24

class SelectPlayer:public CGameObject
{
public:
	int select = 0;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};

