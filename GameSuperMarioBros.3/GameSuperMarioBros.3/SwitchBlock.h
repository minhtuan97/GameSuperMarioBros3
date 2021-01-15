#pragma once
#include "GameObject.h"

#define ANIMATION_ID_SWITCHBLOCK	28
#define SWITCHBLOCK_WIDTH  16
#define SWITCHBLOCK_HEIGHT 16

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

