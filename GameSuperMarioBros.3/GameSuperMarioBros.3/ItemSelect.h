#pragma once
#include "GameObject.h"
#include "Font.h"
#include <string>
#define RENDER_TIME 200
#define ItemSelect_BBOX_WIDTH 8
#define ItemSelect_TYPE_NAM 0
#define ItemSelect_TYPE_SAO 2
#define ItemSelect_TYPE_BONG 1
#define ItemSelect_BBOX_HEIGHT 16
#define ItemSelect_BBOX_HEIGHT 16
#define ITEMSELECT_SPEED_Y	0.1f

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

