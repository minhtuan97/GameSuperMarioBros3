#pragma once
#include "GameObject.h"
class SwitchBlock :
    public CGameObject
{
public:
	bool isSwitch = false;
	bool isColi = false;
	SwitchBlock(float x, float y);
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
};

