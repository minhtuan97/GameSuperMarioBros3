#pragma once
#include "GameObject.h"
#include "Font.h"
#include <string>
#define RENDER_TIME 200

class ItemSelect:public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	bool isColi = false;
	DWORD ani_start;
	int type = 0;
	ItemSelect();
	int Get_animationset_at_k();
};

